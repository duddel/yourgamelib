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
#ifndef YOURGAME_FILEIO_H
#define YOURGAME_FILEIO_H

#include <cstdint>
#include <vector>

namespace yourgame
{
    /** \brief reads an entire file (from asset file location) and returns it as a vector of bytes */
    int readAssetFile(const char *filename, std::vector<uint8_t> &dst);

    /** \brief reads an entire file (from save file location) and returns it as a vector of bytes */
    int readSaveFile(const char *filename, std::vector<uint8_t> &dst);

    /** \brief writes numBytes bytes from data to a file called filename in the save file location.
        overwrites the file if it exists */
    int writeSaveFile(const char *filename, const void *data, size_t numBytes);

    /** \brief returns the absolute path to the save file filename */
    void saveFilePath(const char *filename, std::string &dst);
} // namespace yourgame

#endif
