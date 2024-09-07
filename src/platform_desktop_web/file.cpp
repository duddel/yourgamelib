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
#include <algorithm> // std::sort()
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

namespace
{
    std::string basePathAbs = "";
    std::string assetPathAbs = "";
    std::string projectPath = "";

#ifdef __EMSCRIPTEN__
    int wgetData(const std::string &filepath, std::vector<uint8_t> &dst)
    {
        void *data;
        int size;
        int error;
        emscripten_wget_data(filepath.c_str(), &data, &size, &error);

        if (error == 0)
        {
            dst.resize(size);
            std::memcpy(&dst[0], data, size);
            std::free(data);
        }

        return error;
    }
#endif
} // namespace

namespace yourgame_internal
{
    namespace file
    {
        namespace desktop
        {
            void initFile()
            {
#ifdef __EMSCRIPTEN__
                assetPathAbs = "assets/";
                basePathAbs = "";
#else
                int exeBasePathLength;
                int exePathLength = wai_getExecutablePath(NULL, 0, NULL);
                char *path = (char *)malloc(exePathLength + 1);
                wai_getExecutablePath(path, exePathLength, &exeBasePathLength);
                path[exeBasePathLength + 1] = '\0';

                basePathAbs = std::string(path);

                free(path);
                yourgame_internal::file::normalizePath(basePathAbs);

                // check existing (via dirent openable) asset directories
                for (const auto &a : {"assets/", "../assets/", "../../assets/", "../../../assets/"})
                {
                    assetPathAbs = basePathAbs + a;
                    DIR *dir = opendir(assetPathAbs.c_str());
                    if (dir)
                    {
                        closedir(dir);
                        break;
                    }
                }
#endif
            }
        } // namespace desktop
    } // namespace file
} // namespace yourgame_internal

namespace yourgame
{
    namespace file
    {
        std::string getAssetFilePath(const std::string &pathRelative)
        {
            return assetPathAbs + pathRelative;
        }

        std::string getBasePath(const std::string &pathRelative)
        {
            return basePathAbs + pathRelative;
        }

        int readAssetFile(const std::string &filename, std::vector<uint8_t> &dst)
        {
#ifdef __EMSCRIPTEN__
            return wgetData(assetPathAbs + filename, dst);
#else
            return (yourgame_internal::file::readFileFromPath(assetPathAbs + filename, dst));
#endif
        }

        int readProjectFile(const std::string &filename, std::vector<uint8_t> &dst)
        {
            // Assuming projectPath points to an archive file
            if (getFileExtension(projectPath) == yourgame_internal::file::ARCHIVE_FILE_EXTENSION)
            {
                return yourgame_internal::file::readFileFromArchive(projectPath, filename, dst);
            }

#ifdef __EMSCRIPTEN__
            return wgetData(projectPath + filename, dst);
#else
            return yourgame_internal::file::readFileFromPath(projectPath + filename, dst);
#endif
        }

        void setProjectPath(const std::string &path)
        {
            if (path.size() < 1)
            {
                return;
            }

            projectPath = path;
            yourgame_internal::file::normalizePath(projectPath);

            // Assuming directory path. Append '/'
            if (getFileExtension(projectPath) != yourgame_internal::file::ARCHIVE_FILE_EXTENSION)
            {
                if (projectPath.back() != '/')
                {
                    projectPath += '/';
                }
            }
            // Assuming archive path. On platform web: if the project path is an archive,
            // we download it to transient memory and adjust the projectPath
            else
            {
#ifdef __EMSCRIPTEN__
                std::vector<uint8_t> archiveData;
                if (wgetData(projectPath, archiveData) == 0)
                {
                    projectPath = "/home/web_user/" + getFileName(projectPath);

                    yourgame_internal::file::writeFileToPath(projectPath,
                                                             archiveData.data(),
                                                             archiveData.size());

                    yourgame::log::info("setProjectPath(): saved project archive file to %v", projectPath);
                }
                else
                {
                    yourgame::log::error("setProjectPath(): failed to save project archive file %v", getFileName(projectPath));
                }
#endif
            }
        }

        std::string getProjectFilePath(const std::string &pathRelative)
        {
            return projectPath + pathRelative;
        }

        int writeProjectFile(const std::string &filename, const void *data, size_t numBytes)
        {
            return yourgame_internal::file::writeFileToPath(projectPath + filename, data, numBytes);
        }

        int writeAssetFile(const std::string &filename, const void *data, size_t numBytes)
        {
            return yourgame_internal::file::writeFileToPath(assetPathAbs + filename, data, numBytes);
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
                    pathToOpen = (assetPathAbs + pathToOpen.substr(3, std::string::npos));
                    break;
                case 'p':
                    // substr() returns empty string if pos == length.
                    pathToOpen = (projectPath + pathToOpen.substr(3, std::string::npos));
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
    } // namespace file
} // namespace yourgame
