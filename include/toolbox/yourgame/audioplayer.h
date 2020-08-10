/*
Copyright (c) 2019-2020 Alexander Scholz

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
#ifndef YOURGAME_AUDIOPLAYER_H
#define YOURGAME_AUDIOPLAYER_H

#include <string>
#include <vector>
#include "miniaudio.h"

namespace yourgame
{
    int audioInit(ma_uint32 numChannels, ma_uint32 sampleRate, int numSources);
    void audioShutdown();
    bool audioIsInitialized();
    int audioStoreFile(std::string filename);

    int audioPlay(std::string filename, bool loop = false);
    int audioStop(int sourceId);
    int audioPause(int sourceId, bool pause);
    int audioSetChannelGains(int sourceId, const std::vector<float> &gains);
} // namespace yourgame

#endif
