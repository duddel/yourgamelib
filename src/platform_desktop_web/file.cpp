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
#include <algorithm> // std::replace(), std::sort()
#include <string>
#include <vector>
#include <regex>
#include "dirent.h"
#ifndef __EMSCRIPTEN__
#include "whereami.h"
#endif
#include "yourgame/log.h"
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
        assetPathAbs = "/assets/";
        yourgame_internal::saveFilesPathAbs = "/home/web_user/";
        yourgame_internal::projectPathAbs = yourgame_internal::saveFilesPathAbs;
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
        for (const auto &a : {"assets/", "../assets/", "../../assets/"})
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
        yourgame_internal::projectPathAbs = yourgame_internal::saveFilesPathAbs;
#endif
    }
} // namespace yourgame_internal_desktop

namespace yourgame
{
    namespace file
    {
        int readAssetFile(const std::string &filename, std::vector<uint8_t> &dst)
        {
            return yourgame_internal::readFileFromPath(yourgame_internal_desktop::assetPathAbs + filename, dst);
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
