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
#include <algorithm> // std::replace()
#include <string>
#include <vector>
#ifndef __EMSCRIPTEN__
#include "whereami.h"
#endif
#include "yourgame_internal/file.h"

namespace yourgame_internal_desktop
{
    namespace
    {
        std::string assetPath = "";
        std::string saveFilesPath = "";
    } // namespace

    void initFileIO()
    {
#ifndef __EMSCRIPTEN__
        int exeBasePathLength;
        int exePathLength = wai_getExecutablePath(NULL, 0, NULL);
        char *path = (char *)malloc(exePathLength + 1);
        wai_getExecutablePath(path, exePathLength, &exeBasePathLength);
        path[exeBasePathLength + 1] = '\0';
        assetPath = path;
        free(path);
        std::replace(assetPath.begin(), assetPath.end(), '\\', '/');
        saveFilesPath = assetPath;
        assetPath += "assets/";
        saveFilesPath += "savefiles/";
#else
        assetPath = "/assets/";
        saveFilesPath = "/home/web_user/";
#endif
    }
} // namespace yourgame_internal_desktop

namespace yourgame
{
    int readAssetFile(const char *filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile((yourgame_internal_desktop::assetPath + filename).c_str(), dst);
    }

    int readSaveFile(const char *filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile((yourgame_internal_desktop::saveFilesPath + filename).c_str(), dst);
    }

    int writeSaveFile(const char *filename, const void *data, size_t numBytes)
    {
        return yourgame_internal::writeFile((yourgame_internal_desktop::saveFilesPath + filename).c_str(), data, numBytes);
    }

    void saveFilePath(const char *filename, std::string &dst)
    {
        dst = (yourgame_internal_desktop::saveFilesPath + filename);
    }
} // namespace yourgame
