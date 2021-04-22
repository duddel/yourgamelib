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
#ifndef YOURGAME_FILEIO_H
#define YOURGAME_FILEIO_H

#include <cstdint>
#include <string>
#include <vector>

namespace yourgame
{
    /** \brief reads an entire file (from asset file location) and returns it as a vector of bytes */
    int readAssetFile(const std::string &filename, std::vector<uint8_t> &dst);

    /** \brief reads an entire file (from save file location) and returns it as a vector of bytes */
    int readSaveFile(const std::string &filename, std::vector<uint8_t> &dst);

    /** \brief reads an entire file (from project file location) and returns it as a vector of bytes */
    int readProjectFile(const std::string &filename, std::vector<uint8_t> &dst);

    /** \brief sets the path to the current project directory */
    void setProjectPath(const std::string &path);

    /** \brief reads an entire file and returns it as a vector of bytes
    - prepend \c a// to load a file from asset file location
    - prepend \c s// to load a file from save file location
    - prepend \c p// to load a file from project file location
    - prepend none of the above to load a file with unchanged filename as path
    - example: <code>loadFile("a//file1.bin" ...) // loads file1.bin from assets</code>
    - example: <code>loadFile("s//file2.bin" ...) // loads file2.bin from save file location</code>
    - example: <code>loadFile("p//file3.bin" ...) // loads file3.bin from project file location</code>
    - example: <code>loadFile("file4.bin" ...) // loads file4.bin from working directory </code>
    */
    int readFile(const std::string &filename, std::vector<uint8_t> &dst);

    /** \brief writes numBytes bytes from data to a file called filename in the save file location.
        overwrites the file if it exists */
    int writeSaveFile(const std::string &filename, const void *data, size_t numBytes);

    /** \brief writes numBytes bytes from data to a file called filename in the project file location.
        overwrites the file if it exists */
    int writeProjectFile(const std::string &filename, const void *data, size_t numBytes);
} // namespace yourgame

#endif
