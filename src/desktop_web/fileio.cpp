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
#include <regex>
#include "dirent.h"
#ifndef __EMSCRIPTEN__
#include "whereami.h"
#endif
#include "yourgame/logging.h"
#include "yourgame_internal/file.h"

namespace yourgame_internal_desktop
{
    namespace
    {
        std::string assetPathAbs;
        std::string saveFilesPathAbs;
        std::string projectPathAbs;
    } // namespace

    void initFileIO()
    {
#ifdef __EMSCRIPTEN__
        assetPathAbs = "/assets/";
        saveFilesPathAbs = "/home/web_user/";
        projectPathAbs = saveFilesPathAbs;
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

        saveFilesPathAbs = basePath + "savefiles/";
        projectPathAbs = saveFilesPathAbs;
#endif

        yourgame::logi("File IO assets:     %v", assetPathAbs);
        yourgame::logi("File IO save files: %v", saveFilesPathAbs);
        yourgame::logi("File IO project:    %v", projectPathAbs);
    }
} // namespace yourgame_internal_desktop

namespace yourgame
{
    std::string getSaveFilePath(const std::string &filename)
    {
        return yourgame_internal_desktop::saveFilesPathAbs + filename;
    }

    std::string getProjectFilePath(const std::string &filename)
    {
        return yourgame_internal_desktop::projectPathAbs + filename;
    }

    int readAssetFile(const std::string &filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile(yourgame_internal_desktop::assetPathAbs + filename, dst);
    }

    int readSaveFile(const std::string &filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile(yourgame_internal_desktop::saveFilesPathAbs + filename, dst);
    }

    int readProjectFile(const std::string &filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile(yourgame_internal_desktop::projectPathAbs + filename, dst);
    }

    void setProjectPath(const std::string &path)
    {
        yourgame_internal_desktop::projectPathAbs = path;
        std::replace(yourgame_internal_desktop::projectPathAbs.begin(), yourgame_internal_desktop::projectPathAbs.end(), '\\', '/');
        if (yourgame_internal_desktop::projectPathAbs.back() != '/')
        {
            yourgame_internal_desktop::projectPathAbs += "/";
        }
        yourgame::logd("File IO project: %v", yourgame_internal_desktop::projectPathAbs);
    }

    int readFile(const std::string &filename, std::vector<uint8_t> &dst)
    {
        if (filename.length() > 3 && filename.compare(1, 2, "//") == 0)
        {
            switch (filename[0])
            {
            case 'a':
                return readAssetFile(filename.substr(3, std::string::npos), dst);
            case 's':
                return readSaveFile(filename.substr(3, std::string::npos), dst);
            case 'p':
                return readProjectFile(filename.substr(3, std::string::npos), dst);
            }
        }

        return yourgame_internal::readFile(filename, dst);
    }

    int writeSaveFile(const std::string &filename, const void *data, size_t numBytes)
    {
        return yourgame_internal::writeFile(yourgame_internal_desktop::saveFilesPathAbs + filename, data, numBytes);
    }

    int writeProjectFile(const std::string &filename, const void *data, size_t numBytes)
    {
        return yourgame_internal::writeFile(yourgame_internal_desktop::projectPathAbs + filename, data, numBytes);
    }

    std::vector<std::string> ls(const std::string &pattern)
    {
        std::vector<std::string> ret;

        std::string lsPath = pattern;
        if (lsPath.length() >= 3 && lsPath.compare(1, 2, "//") == 0)
        {
            switch (lsPath[0])
            {
            case 'a':
                // substr() returns empty string if pos == length.
                lsPath = (yourgame_internal_desktop::assetPathAbs + lsPath.substr(3, std::string::npos));
                break;
            case 's':
                // substr() returns empty string if pos == length.
                lsPath = (yourgame_internal_desktop::saveFilesPathAbs + lsPath.substr(3, std::string::npos));
                break;
            case 'p':
                // substr() returns empty string if pos == length.
                lsPath = (yourgame_internal_desktop::projectPathAbs + lsPath.substr(3, std::string::npos));
                break;
            }
        }

        // match 0: full string, 1: beginning until last /, 2: everything after last /
        static std::regex reLsPattern("(.*\\/|^)(.*\\*[^\\/\\n]*)$");
        std::smatch reMatch;
        auto reMatched = std::regex_match(lsPath, reMatch, reLsPattern);

        yourgame::logd("ls(): lsPath matched: %v, matches: %v:", reMatched, reMatch.size());
        for (unsigned i = 0; i < reMatch.size(); i++)
        {
            yourgame::logd("ls(): match %v: %v", i, reMatch[i].str());
        }

        std::regex reFilePattern("^.*$");
        if (reMatched && reMatch.size() == 3)
        {
            lsPath = reMatch[1].str();
            std::string filePattern = reMatch[2].str();
            filePattern = std::regex_replace(filePattern, std::regex("\\."), "\\.");
            filePattern = std::regex_replace(filePattern, std::regex("\\*"), ".*");
            yourgame::logd("ls(): filePattern regex: %v", filePattern);
            // todo ensure filePattern is a valid regex
            reFilePattern = std::regex(std::string("^") + filePattern + "$");
        }

        DIR *dir = opendir(lsPath.c_str());
        if (!dir)
        {
            yourgame::loge("ls(): failed to open directory: %v", lsPath);
        }
        else
        {
            struct dirent *ent;
            while ((ent = readdir(dir)) != NULL)
            {
                if (std::regex_match(ent->d_name, reFilePattern))
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

        return ret;
    }
} // namespace yourgame
