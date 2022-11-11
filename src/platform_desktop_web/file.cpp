/*
Copyright (c) 2019-2023 Alexander Scholz

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
#include <algorithm> // std::replace(), std::sort()
#include <string>
#include <vector>
#include <regex>
#include "dirent.h"
#ifndef __EMSCRIPTEN__
#include "whereami.h"
#else
#include <emscripten/emscripten.h>
#endif
#include "yourgame/log.h"
#include "yourgame/file.h"
#include "yourgame_internal/file.h"

namespace yourgame_internal_desktop
{
    namespace
    {
        std::string assetPathAbs;
    } // namespace

    void initFile()
    {
#ifdef __EMSCRIPTEN__
        assetPathAbs = "/home/web_user/";
        yourgame_internal::saveFilesPathAbs = "/home/web_user/";
        yourgame_internal::projectPathAbs = "";
#else
        int exeBasePathLength;
        int exePathLength = wai_getExecutablePath(NULL, 0, NULL);
        char *path = (char *)malloc(exePathLength + 1);
        wai_getExecutablePath(path, exePathLength, &exeBasePathLength);
        path[exeBasePathLength + 1] = '\0';
        std::string basePath(path);
        free(path);
        std::replace(basePath.begin(), basePath.end(), '\\', '/');

        // check existing (via dirent openable) asset directories
        for (const auto &a : {"assets/", "../assets/", "../../assets/", "../../../assets/"})
        {
            assetPathAbs = basePath + a;
            DIR *dir = opendir(assetPathAbs.c_str());
            if (dir)
            {
                closedir(dir);
                break;
            }
        }

        yourgame_internal::saveFilesPathAbs = basePath + "savefiles/";
        yourgame_internal::projectPathAbs = "";
#endif
    }
} // namespace yourgame_internal_desktop

namespace yourgame
{
    namespace file
    {
        int readAssetFile(const std::string &filename, std::vector<uint8_t> &dst)
        {
            // on platform desktop: try to read file from the actual assets directory.
            // on platform web, this is transient memory. assets are only available if they
            // have been packed by emscripten or have been downloaded before (see below).
            int ret = yourgame_internal::readFileFromPath(yourgame_internal_desktop::assetPathAbs + filename, dst);
            if (ret == 0)
            {
                yourgame::log::debug("readAssetFile(): successfully read asset %v from %v", filename, yourgame_internal_desktop::assetPathAbs);
            }

// on platform desktop, return result of readFileFromPath()
#ifndef __EMSCRIPTEN__
            return ret;
// on platform web, try to download (wget) the file
#else
            if (ret != 0)
            {
                for (const auto &a : {"./assets/", "../assets/"})
                {
                    void *wgetData;
                    int wgetSize;
                    int wgetError;

                    emscripten_wget_data((a + filename).c_str(), &wgetData, &wgetSize, &wgetError);

                    if (wgetError == 0)
                    {
                        yourgame::log::debug("readAssetFile(): successfully downloaded asset %v from %v, %v bytes", filename, a, wgetSize);

                        // save downloaded asset in asset directory
                        if (yourgame::file::writeAssetFile(filename, wgetData, wgetSize) == 0)
                        {
                            yourgame::log::debug("readAssetFile() successfully saved asset %v", filename);
                        }
                        else
                        {
                            yourgame::log::debug("readAssetFile() failed to save asset %v", filename);
                        }

                        dst.resize(wgetSize);
                        std::memcpy(&dst[0], wgetData, wgetSize);
                        std::free(wgetData);
                        ret = 0;
                        break;
                    }
                }
            }

            return ret;
#endif
        }

        int writeAssetFile(const std::string &filename, const void *data, size_t numBytes)
        {
            return yourgame_internal::writeFileToPath(yourgame_internal_desktop::assetPathAbs + filename, data, numBytes);
        }

        std::vector<std::string> ls(const std::string &pattern)
        {
            std::vector<std::string> ret;
            std::string pathToOpen = pattern;

            // step 1: if pattern has a file location prefix (like a//),
            // replace it with the appropriate absolut path
            if (pathToOpen.length() >= 3 && pathToOpen.compare(1, 2, "//") == 0)
            {
                switch (pathToOpen[0])
                {
                case 'a':
                    // substr() returns empty string if pos == length.
                    pathToOpen = (yourgame_internal_desktop::assetPathAbs + pathToOpen.substr(3, std::string::npos));
                    break;
                case 's':
                    // substr() returns empty string if pos == length.
                    pathToOpen = (yourgame_internal::saveFilesPathAbs + pathToOpen.substr(3, std::string::npos));
                    break;
                case 'p':
                    // substr() returns empty string if pos == length.
                    pathToOpen = (yourgame_internal::projectPathAbs + pathToOpen.substr(3, std::string::npos));
                    break;
                }
            }

            std::regex reFileFilter("^.*$");

            // step 2: check if pathToOpen has one or more "*" after the last "/".
            //   if so, use that last part as a file filter, and everything until the last "/" as the actual path to open.
            //   otherwise, leave pathToOpen unchanged.
            static std::regex reLsPattern(R"((.*\/|^)(.*\*[^\/\n]*)$)"); // match 0: full string,
                                                                         // 1: beginning until last "/",
                                                                         // 2: everything after last "/" (if it contains one or more "*")
            std::smatch reMatches;
            std::string tmp = pathToOpen;
            if (std::regex_match(tmp, reMatches, reLsPattern) && reMatches.size() == 3)
            {
                pathToOpen = reMatches[1].str();
                std::string filePattern = reMatches[2].str();
                filePattern = std::regex_replace(filePattern, std::regex("\\."), "\\.");
                filePattern = std::regex_replace(filePattern, std::regex("\\*"), ".*");
                // todo ensure filePattern is a valid regex
                reFileFilter = std::regex(std::string("^") + filePattern + "$");
            }

            // try to open the path
            DIR *dir = opendir(pathToOpen.c_str());
            if (!dir)
            {
                yourgame::log::error("ls(): failed to open directory: %v", pathToOpen);
            }
            else
            {
                struct dirent *ent;
                while ((ent = readdir(dir)) != NULL)
                {
                    if (std::regex_match(ent->d_name, reFileFilter))
                    {
                        switch (ent->d_type)
                        {
                        case DT_REG:
                            ret.emplace_back(std::string(ent->d_name));
                            break;
                        case DT_DIR:
                            ret.emplace_back(std::string(ent->d_name) + "/");
                            break;
                        case DT_LNK:
                            ret.emplace_back(std::string(ent->d_name) + "@");
                            break;
                        default:
                            ret.emplace_back(std::string(ent->d_name) + "*");
                        }
                    }
                }

                closedir(dir);
            }

            std::sort(ret.begin(), ret.end());
            return ret;
        }
    }
}
