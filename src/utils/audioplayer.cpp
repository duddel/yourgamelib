/*
Copyright (c) 2019-2021 Alexander Scholz

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#include <string>
#include <map>
#include <cmath>
#include <cstdint>
#include <vector>
#include <valarray>
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#include "miniaudio.h"
#include "yourgame/fileio.h"

namespace yourgame
{
    namespace
    {
        struct audioSource
        {
            ma_decoder decoder;
            std::vector<float> channelGains;
            bool paused = false;
            bool loop = false;
        };

        ma_device g_maDevice;
        std::vector<audioSource *> g_sources;
        ma_mutex g_mtxSources;
        std::map<std::string, std::vector<uint8_t>> g_audioFileData;
        bool g_audioInitialized = false;

        // does NOT lock g_mtxSources
        int deleteAudioSource(int sourceId)
        {
            if (sourceId >= g_sources.size())
            {
                return -1;
            }

            audioSource *&src = g_sources[sourceId];
            if (src == nullptr)
            {
                return -2;
            }

            ma_decoder_uninit(&(src->decoder));
            delete src;
            src = nullptr;
            return 0;
        }

        void maDataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
        {
            ma_uint32 numDeviceChannels = pDevice->playback.channels;

            ma_mutex_lock(&g_mtxSources);
            for (auto iSrc = 0; iSrc < g_sources.size(); iSrc++)
            {
                audioSource *src = g_sources[iSrc];

                if (src == nullptr || src->paused)
                {
                    continue;
                }

                // prepare storage for pcm frames
                std::vector<float> pcmBuf(frameCount * numDeviceChannels, 0.0f);

                // read pcm frames
                ma_uint64 framesBuffered;
                auto maRes = ma_data_source_read_pcm_frames(&(src->decoder), pcmBuf.data(), frameCount, &framesBuffered, src->loop);

                // form a valarray from pcm frames. the gains get applied in this valarray below
                std::valarray<float> pcmBufVarr(pcmBuf.data(), pcmBuf.size());

                for (auto iChan = 0; iChan < numDeviceChannels; iChan++)
                {
                    // skip channel if gain is unchanged (1.0)
                    if (std::abs(src->channelGains[iChan] - 1.0f) < 0.0e-6f)
                    {
                        continue;
                    }

                    // apply gains to the channel slice of the valarray. this manipulates the original valarray data
                    std::slice_array<float> pcmBufChan = pcmBufVarr[std::slice(iChan, framesBuffered, numDeviceChannels)];
                    pcmBufChan *= std::valarray<float>(src->channelGains[iChan], framesBuffered);
                }

                // write valarray with applied gains back to the pcm buffer
                pcmBuf = std::vector<float>(std::begin(pcmBufVarr), std::end(pcmBufVarr));

                // mix pcm frames with the actual pOutput
                for (auto i = 0; i < framesBuffered * numDeviceChannels; i++)
                {
                    ((float *)pOutput)[i] += pcmBuf[i];
                }

                // a non-looping source is deleted when end of data is reached
                if (framesBuffered < frameCount && !src->loop)
                {
                    deleteAudioSource(iSrc);
                }
            }

            ma_mutex_unlock(&g_mtxSources);
            (void)pInput;
        }
    } // namespace

    int audioInit(ma_uint32 numChannels, ma_uint32 sampleRate, int numSources)
    {
        // audio sources are identified by their index. the size of
        // this vector is unchanged once initialized (resized) here:
        g_sources.resize(numSources, nullptr);

        // create miniaudio device
        ma_device_config maDeviceConfig = ma_device_config_init(ma_device_type_playback);
        // the only supported format is ma_format_f32.
        // calculations in maDataCallback() are settled for this format
        // todo: check if a different format might be desired
        maDeviceConfig.playback.format = ma_format_f32;
        maDeviceConfig.playback.channels = numChannels;
        maDeviceConfig.sampleRate = sampleRate;
        maDeviceConfig.dataCallback = maDataCallback;
        maDeviceConfig.pUserData = NULL;

        if (ma_device_init(NULL, &maDeviceConfig, &g_maDevice) != MA_SUCCESS)
        {
            return -2;
        }

        if (ma_device_start(&g_maDevice) != MA_SUCCESS)
        {
            ma_device_uninit(&g_maDevice);
            return -3;
        }

        g_audioInitialized = true;
        return 0;
    }

    void audioShutdown()
    {
        g_audioFileData.clear();
        ma_device_uninit(&g_maDevice);

        // delete all audio sources
        ma_mutex_lock(&g_mtxSources);
        for (auto i = 0; i < g_sources.size(); i++)
        {
            deleteAudioSource(i);
        }
        ma_mutex_unlock(&g_mtxSources);

        g_audioInitialized = false;
    }

    bool audioIsInitialized()
    {
        return g_audioInitialized;
    }

    int audioStoreFile(const std::string &filename)
    {
        if (g_audioFileData.find(filename) != g_audioFileData.end())
        {
            return -1;
        }

        std::vector<uint8_t> fileData;
        if (yourgame::readFile(filename, fileData) == 0)
        {
            g_audioFileData.emplace(filename, fileData);
            return 0;
        }
        else
        {
            return -2;
        }
    }

    int audioPlay(const std::string &filename, bool loop)
    {
        auto audioFile = g_audioFileData.find(filename);
        if (audioFile == g_audioFileData.end())
        {
            return -1;
        }

        // check if an empty source slot is available
        int sourceId;
        bool emptySourceSlot = false;
        ma_mutex_lock(&g_mtxSources);
        for (sourceId = 0; sourceId < g_sources.size(); sourceId++)
        {
            if (g_sources[sourceId] == nullptr)
            {
                emptySourceSlot = true;
                break;
            }
        }
        ma_mutex_unlock(&g_mtxSources);

        if (!emptySourceSlot)
        {
            return -2;
        }

        // adjust the decoder config by the device config to have the audio data
        // converted during decoding, to match the actual device
        ma_decoder_config maDecCfg = ma_decoder_config_init(
            g_maDevice.playback.format,
            g_maDevice.playback.channels,
            g_maDevice.sampleRate);

        audioSource *newSource = new audioSource();
        if (ma_decoder_init_memory_vorbis(&audioFile->second[0], audioFile->second.size(), &maDecCfg, &(newSource->decoder)) != MA_SUCCESS)
        {
            delete newSource;
            return -3;
        }

        newSource->channelGains.resize(g_maDevice.playback.channels, 1.0f);
        newSource->loop = loop;

        ma_mutex_lock(&g_mtxSources);
        g_sources[sourceId] = newSource;
        ma_mutex_unlock(&g_mtxSources);

        return sourceId;
    }

    int audioStop(int sourceId)
    {
        ma_mutex_lock(&g_mtxSources);
        int ret = deleteAudioSource(sourceId);
        ma_mutex_unlock(&g_mtxSources);
        return ret;
    }

    int audioPause(int sourceId, bool pause)
    {
        int ret = 0;

        ma_mutex_lock(&g_mtxSources);
        if (sourceId >= g_sources.size())
        {
            ret = -1;
        }

        if (g_sources[sourceId] != nullptr)
        {
            g_sources[sourceId]->paused = pause;
        }
        else
        {
            ret = -2;
        }
        ma_mutex_unlock(&g_mtxSources);

        return ret;
    }

    int audioSetChannelGains(int sourceId, const std::vector<float> &gains)
    {
        int ret = 0;

        ma_mutex_lock(&g_mtxSources);
        if (sourceId >= g_sources.size())
        {
            ret = -1;
        }

        if (g_sources[sourceId] != nullptr)
        {
            g_sources[sourceId]->channelGains = gains;
        }
        else
        {
            ret = -2;
        }
        ma_mutex_unlock(&g_mtxSources);

        return ret;
    }
} // namespace yourgame
