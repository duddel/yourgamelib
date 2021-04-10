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
#include <algorithm> // std::replace()
#include <string>
#include <vector>
#ifndef __EMSCRIPTEN__
#include "whereami.h"
#endif
#include "yourgame/logging.h"
#include "yourgame_internal/file.h"

namespace yourgame_internal_desktop
{
    namespace
    {
#ifdef __EMSCRIPTEN__
        std::string basePath = "/";
        std::string saveFilesPath = "home/web_user/";
        std::vector<std::string> assetPaths = {"assets/"};
#else
        std::string basePath = "";
        std::string saveFilesPath = "savefiles/";
        std::vector<std::string> assetPaths = {"assets/", "../assets/", "../../assets/"};
#endif
    } // namespace

    void initFileIO()
    {
#ifndef __EMSCRIPTEN__
        int exeBasePathLength;
        int exePathLength = wai_getExecutablePath(NULL, 0, NULL);
        char *path = (char *)malloc(exePathLength + 1);
        wai_getExecutablePath(path, exePathLength, &exeBasePathLength);
        path[exeBasePathLength + 1] = '\0';
        basePath = path;
        free(path);
        std::replace(basePath.begin(), basePath.end(), '\\', '/');
#endif
        yourgame::logi("File IO basePath: %v", basePath);
    }
} // namespace yourgame_internal_desktop

namespace yourgame
{
    int readAssetFile(const char *filename, std::vector<uint8_t> &dst)
    {
        int readFileRet = -1;
        for (const auto &aP : yourgame_internal_desktop::assetPaths)
        {
            auto assetFilePath = (yourgame_internal_desktop::basePath + aP + filename);
            if ((readFileRet = yourgame_internal::readFile(assetFilePath.c_str(), dst)) == 0)
            {
                yourgame::logd("asset file %v, found here: %v", filename, assetFilePath);
                return readFileRet;
            }
        }
        yourgame::logd("asset file %v, not found", filename);
        return readFileRet;
    }

    std::string saveFilePath(const char *filename)
    {
        return (yourgame_internal_desktop::basePath + yourgame_internal_desktop::saveFilesPath + filename);
    }

    int readSaveFile(const char *filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile(saveFilePath(filename).c_str(), dst);
    }

    int writeSaveFile(const char *filename, const void *data, size_t numBytes)
    {
        return yourgame_internal::writeFile(saveFilePath(filename).c_str(), data, numBytes);
    }
} // namespace yourgame
