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
#ifndef YOURGAME_FILE_H
#define YOURGAME_FILE_H

#include <cstdint>
#include <string>
#include <vector>

namespace yourgame
{
    namespace file
    {
        /** \brief returns the full (absolut) path to a file called filename in the save file location */
        std::string getSaveFilePath(const std::string &filename = "");

        /** \brief returns the full (absolut) path to a file called filename in the project file location */
        std::string getProjectFilePath(const std::string &filename = "");

        /** \brief reads an entire file (from asset file location) and returns it as a vector of bytes */
        int readAssetFile(const std::string &filename, std::vector<uint8_t> &dst);

        /** \brief reads an entire file (from save file location) and returns it as a vector of bytes */
        int readSaveFile(const std::string &filename, std::vector<uint8_t> &dst);

        /** \brief reads an entire file (from project file location) and returns it as a vector of bytes */
        int readProjectFile(const std::string &filename, std::vector<uint8_t> &dst);

        /** \brief sets path as current project directory. initial project directory is "" */
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

        /** \brief returns the location (path or prefix, such as a//) of a file, filepath points to
        - example: "file1.bin" returns ""
        - example: "a//file2.bin" returns "a//"
        - example: "/home/user/file3.bin" returns "/home/user/"
        */
        std::string getFileLocation(const std::string &filepath);

        /** \brief returns the actual file name of a file, filepath points to
        - example: "file1.bin" returns "file1.bin"
        - example: "a//file2.bin" returns "file2.bin"
        - example: "/home/user/file3.bin" returns "file3.bin"
        */
        std::string getFileName(const std::string &filepath);

        /** \brief returns the actual file name (w/o file extension) of a file, filepath points to
        - example: "file1.bin" returns "file1"
        - example: "a//file2.bin" returns "file2"
        - example: "/home/user/file3.bin" returns "file3"
        - example: "file4" returns "file4"
        - example: ".file5" returns ".file5"
        - example: "a//file6.tar.gz" returns "file6"
        - example: "a//file7" returns "file7"
        - example: "/home/user.a/file8.txt" returns "file8"
        */
        std::string getFileNameWithoutExtension(const std::string &filepath);

        /** \brief returns the file extension
        - example: "file1.bin" returns "bin"
        - example: "a//texture.png" returns "png"
        - example: ".gitignore" returns "gitignore"
        - example: "Makefile" returns ""
        */
        std::string getFileExtension(const std::string &filepath);

        /** \brief writes numBytes bytes from data to a file called filename in the asset file location.
        overwrites the file if it exists */
        int writeAssetFile(const std::string &filename, const void *data, size_t numBytes);

        /** \brief writes numBytes bytes from data to a file called filename in the save file location.
        overwrites the file if it exists */
        int writeSaveFile(const std::string &filename, const void *data, size_t numBytes);

        /** \brief writes numBytes bytes from data to a file called filename in the project file location.
        overwrites the file if it exists */
        int writeProjectFile(const std::string &filename, const void *data, size_t numBytes);

        /** \brief writes numBytes bytes from data to a file called filename, destination path determined
        by prefix ("a//", etc.), see readFile(). overwrites the file if it exists.
        \attention filename requires a prefix, such as "a//". otherwise, writing is omitted. */
        int writeFile(const std::string &filename, const void *data, size_t numBytes);

        /** \brief returns list of directory content. accepts prefixes like readFile() does. accepts wildcard "*" for
        files (after last / in pattern) */
        std::vector<std::string> ls(const std::string &pattern);
    } // namespace file
} // namespace yourgame

#endif
