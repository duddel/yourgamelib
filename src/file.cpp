/*
Copyright (c) 2019-2022 Alexander Scholz

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
#include <regex>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>
#include "yourgame/file.h"
#include "yourgame/log.h"

namespace yourgame_internal
{
    std::string saveFilesPathAbs = "";
    std::string projectPathAbs = "";

    int readFileFromPath(const std::string &filepath, std::vector<uint8_t> &dst)
    {
        std::FILE *f = std::fopen(filepath.c_str(), "rb");
        if (f)
        {
            std::fseek(f, 0, SEEK_END);
            auto nBytes = std::ftell(f);
            dst.resize(nBytes);
            std::rewind(f);
            std::fread(&dst[0], 1, dst.size(), f);
            std::fclose(f);
            return 0;
        }
        return -1;
    }

    int writeFileToPath(const std::string &filepath, const void *data, size_t numBytes)
    {
        int ret = -1;
        yourgame::log::debug("writeFileToPath(): opening in wb mode: %v", filepath);
        std::FILE *f = std::fopen(filepath.c_str(), "wb");
        if (f)
        {
            std::fwrite(data, 1, numBytes, f);
            ret = std::ferror(f);
            std::fclose(f);
        }
        return ret;
    }
} // namespace yourgame_internal

namespace yourgame
{
    namespace file
    {
        std::string getSaveFilePath(const std::string &filename)
        {
            return yourgame_internal::saveFilesPathAbs + filename;
        }

        std::string getProjectFilePath(const std::string &filename)
        {
            return yourgame_internal::projectPathAbs + filename;
        }

        int readSaveFile(const std::string &filename, std::vector<uint8_t> &dst)
        {
            return yourgame_internal::readFileFromPath(yourgame_internal::saveFilesPathAbs + filename, dst);
        }

        int readProjectFile(const std::string &filename, std::vector<uint8_t> &dst)
        {
            return yourgame_internal::readFileFromPath(yourgame_internal::projectPathAbs + filename, dst);
        }

        void setProjectPath(const std::string &path)
        {
            yourgame_internal::projectPathAbs = path;
            std::replace(yourgame_internal::projectPathAbs.begin(), yourgame_internal::projectPathAbs.end(), '\\', '/');
            if (yourgame_internal::projectPathAbs.back() != '/')
            {
                yourgame_internal::projectPathAbs += "/";
            }
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

            return yourgame_internal::readFileFromPath(filename, dst);
        }

        std::string getFileLocation(const std::string &filepath)
        {
            // return location prefix (a// etc.)
            if (filepath.length() > 3 && filepath.compare(1, 2, "//") == 0)
            {
                return filepath.substr(0, 3);
            }

            // match 1: beginning until last "/"
            static std::regex reFilePath(R"((.*\/|^).*$)");
            std::smatch reMatches;
            if (std::regex_match(filepath, reMatches, reFilePath) && reMatches.size() == 2)
            {
                return reMatches[1].str();
            }

            return "";
        }

        std::string getFileName(const std::string &filepath)
        {
            // match 2: everything after last "/"
            static std::regex reFilePath(R"((.*\/|^)(.*)$)");
            std::smatch reMatches;
            if (std::regex_match(filepath, reMatches, reFilePath) && reMatches.size() == 3)
            {
                return reMatches[2].str();
            }

            return "";
        }

        std::string getFileExtension(const std::string &filepath)
        {
            // match 2: everything after last "."
            static std::regex reFilePath(R"((.*\.)(.*)$)");
            std::smatch reMatches;
            if (std::regex_match(filepath, reMatches, reFilePath) && reMatches.size() == 3)
            {
                return reMatches[2].str();
            }

            return "";
        }

        int writeSaveFile(const std::string &filename, const void *data, size_t numBytes)
        {
            return yourgame_internal::writeFileToPath(yourgame_internal::saveFilesPathAbs + filename, data, numBytes);
        }

        int writeProjectFile(const std::string &filename, const void *data, size_t numBytes)
        {
            return yourgame_internal::writeFileToPath(yourgame_internal::projectPathAbs + filename, data, numBytes);
        }

        int writeFile(const std::string &filename, const void *data, size_t numBytes)
        {
            if (filename.length() > 3 && filename.compare(1, 2, "//") == 0)
            {
                switch (filename[0])
                {
                case 'a':
                    return writeAssetFile(filename.substr(3, std::string::npos), data, numBytes);
                case 's':
                    return writeSaveFile(filename.substr(3, std::string::npos), data, numBytes);
                case 'p':
                    return writeProjectFile(filename.substr(3, std::string::npos), data, numBytes);
                }
            }

            return -1;
        }
    }
}
