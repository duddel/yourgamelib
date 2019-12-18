/*
Copyright (c) 2019 Alexander Scholz

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
#include <algorithm> // std::replace()
#include <fstream>
#include <vector>
#ifndef __EMSCRIPTEN__
#include "whereami.h"
#endif

namespace yourgame
{
namespace
{
std::string _assetPath;
}

void initAssetFile()
{
#ifndef __EMSCRIPTEN__
    int exeBasePathLength;
    int exePathLength = wai_getExecutablePath(NULL, 0, NULL);
    char *path = (char *)malloc(exePathLength + 1);
    wai_getExecutablePath(path, exePathLength, &exeBasePathLength);
    path[exeBasePathLength + 1] = '\0';
    _assetPath = path;
    free(path);
    std::replace(_assetPath.begin(), _assetPath.end(), '\\', '/');
    _assetPath += "assets/";
#else
    _assetPath = "/assets/";
#endif
}

std::vector<uint8_t> readAssetFile(const char *filename)
{
    std::vector<uint8_t> buf{'\0'};
    std::FILE *f = std::fopen((_assetPath + filename).c_str(), "rb");
    if (f)
    {
        std::fseek(f, 0, SEEK_END);
        auto nBytes = std::ftell(f);
        buf.resize(nBytes);
        std::rewind(f);
        std::fread(&buf[0], 1, buf.size(), f);
        std::fclose(f);
    }
    // todo: perform some error checking
    return buf;
}

} // namespace yourgame
