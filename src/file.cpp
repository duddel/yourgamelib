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
#include <algorithm> // std::replace()
#include <regex>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>
#include "dirent.h"
#include "miniz.h"
#include "yourgame/file.h"
#include "yourgame/log.h"

namespace yourgame_internal
{
    namespace file
    {
        int readFileFromPath(const std::string &filepath, std::vector<uint8_t> &dst)
        {
            std::FILE *f = std::fopen(filepath.c_str(), "rb");
            if (f)
            {
                std::fseek(f, 0, SEEK_END);
                auto nBytes = std::ftell(f);
                if (nBytes > 0)
                {
                    dst.resize(nBytes);
                    std::rewind(f);
                    std::fread(&dst[0], 1, dst.size(), f);
                }
                std::fclose(f);
                return 0;
            }
            return -1;
        }

        int readFileFromArchive(const std::string &archivepath, const std::string &filename, std::vector<uint8_t> &dst)
        {
            mz_zip_archive zip_archive;
            void *p;
            size_t uncomp_size;

            std::memset(&zip_archive, 0, sizeof(zip_archive));

            mz_bool status = mz_zip_reader_init_file(&zip_archive, archivepath.c_str(), 0);
            if (!status)
            {
                return -2;
            }

            p = mz_zip_reader_extract_file_to_heap(&zip_archive, filename.c_str(), &uncomp_size, 0);
            if (!p)
            {
                mz_zip_reader_end(&zip_archive);
                return -3;
            }

            // Success
            dst.assign(static_cast<uint8_t *>(p),
                       static_cast<uint8_t *>(p) + uncomp_size);
            mz_free(p);

            return 0;
        }

        int writeFileToPath(const std::string &filepath, const void *data, size_t numBytes)
        {
            int ret = -1;
            std::FILE *f = std::fopen(filepath.c_str(), "wb");
            if (f)
            {
                std::fwrite(data, 1, numBytes, f);
                ret = std::ferror(f);
                std::fclose(f);
            }
            return ret;
        }

        bool checkIfPathIsDirectory(const std::string &filepath)
        {
            DIR *dir = opendir(filepath.c_str());
            if (dir)
            {
                closedir(dir);
                return true;
            }
            return false;
        }

        void normalizePath(std::string &path)
        {
            std::replace(path.begin(), path.end(), '\\', '/');
        }
    } // namespace file
} // namespace yourgame_internal

namespace yourgame
{
    namespace file
    {
        int readFile(const std::string &filename, std::vector<uint8_t> &dst)
        {
            if (filename.length() > 3 && filename.compare(1, 2, "//") == 0)
            {
                switch (filename[0])
                {
                case 'a':
                    return readAssetFile(filename.substr(3, std::string::npos), dst);
                case 'p':
                    return readProjectFile(filename.substr(3, std::string::npos), dst);
                }
            }

            return yourgame_internal::file::readFileFromPath(filename, dst);
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

        std::string getFileNameWithoutExtension(const std::string &filepath)
        {
            // match 2: everything after last "/" and then
            //   rest of filepath if there are no more ".", OR
            //   everything before first ".", OR
            //   file name starting with "."
            static std::regex re(R"((^.*\/|^)([^.]+$|[^.]+|\..+$)(.*$))");
            std::smatch reMatches;
            if (std::regex_match(filepath, reMatches, re) && reMatches.size() == 4)
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

        int writeFile(const std::string &filename, const void *data, size_t numBytes)
        {
            if (filename.length() > 3 && filename.compare(1, 2, "//") == 0)
            {
                switch (filename[0])
                {
                case 'a':
                    return writeAssetFile(filename.substr(3, std::string::npos), data, numBytes);
                case 'p':
                    return writeProjectFile(filename.substr(3, std::string::npos), data, numBytes);
                }
            }

            return -1;
        }
    } // namespace file
} // namespace yourgame
