/*
Copyright (c) 2019-2024 Alexander Scholz

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
#ifndef YOURGAME_AUDIO_H
#define YOURGAME_AUDIO_H

#include <string>
#include <vector>

namespace yourgame
{
    namespace audio
    {
        /**
        \brief initialize audio
         */
        int init(int numSources, int numChannels = 0, int sampleRate = 0);

        /**
        \brief shutdown audio
         */
        void shutdown();

        /**
        \brief check if audio initialized
         */
        bool isInitialized();

        /**
        \brief load and store audio file
         */
        int storeFile(const std::string &filename);

        /**
        \brief play audio file, that was previously loaded via storeFile()

        \param filename
        \param loop
        \return int audio source id
         */
        int play(const std::string &filename, bool loop = false);

        /**
        \brief stop audio source

        \param sourceId audio source id
        \return int
         */
        int stop(int sourceId);

        /**
        \brief pause/unpause audio source

        \param sourceId audio source id
        \param pause true: pause, false: unpause
        \return int
         */
        int pause(int sourceId, bool pause);

        /**
        \brief set gain per channel of an audio source

        \param sourceId audio source id
        \param gains gain [0.0 .. 1.0] per channel
        \return int
         */
        int setChannelGains(int sourceId, const std::vector<float> &gains);
    } // namespace audio
} // namespace yourgame

#endif
