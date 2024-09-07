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
#ifndef YOURGAME_INTERNAL_FILE_H
#define YOURGAME_INTERNAL_FILE_H

#include <cstdint>
#include <vector>
#include <string>

namespace yourgame_internal
{
    namespace file
    {
        const std::string ARCHIVE_FILE_EXTENSION = "zip";

        int readFileFromPath(const std::string &filepath, std::vector<uint8_t> &dst);

        int readFileFromArchive(const std::string &archivepath, const std::string &filename, std::vector<uint8_t> &dst);

        int writeFileToPath(const std::string &filepath, const void *data, size_t numBytes);

        bool checkIfPathIsDirectory(const std::string &filepath);

        void normalizePath(std::string &path);
    } // namespace file
} // namespace yourgame_internal

#endif
