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
#include <string>
#include <vector>
#include <regex>
#include "dirent.h" // todo preparation for ls()
#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include "yourgame_internal/file.h"

namespace yourgame_internal_android
{
    namespace
    {
        AAssetManager *assMan;
        std::string saveFilesPath = "";
        std::string projectPath = "";
    } // namespace

    void initFileIO(struct android_app *app)
    {
        assMan = app->activity->assetManager;
        saveFilesPath = std::string(app->activity->internalDataPath);
        projectPath = saveFilesPath;
    }
} // namespace yourgame_internal_android

namespace yourgame
{
    std::string getSaveFilePath(const std::string &filename)
    {
        return yourgame_internal_android::saveFilesPath + filename;
    }

    std::string getProjectFilePath(const std::string &filename)
    {
        return yourgame_internal_android::projectPath + filename;
    }

    int readAssetFile(const std::string &filename, std::vector<uint8_t> &dst)
    {
        AAsset *assDesc = AAssetManager_open(yourgame_internal_android::assMan,
                                             filename.c_str(),
                                             AASSET_MODE_BUFFER);
        if (assDesc)
        {
            auto nBytes = AAsset_getLength(assDesc);
            dst.resize(nBytes);
            int64_t nBytesRead = AAsset_read(assDesc, dst.data(), dst.size());
            AAsset_close(assDesc);
            return 0;
        }
        // todo: check dst.size() against nBytesRead and handle error case
        return -1;
    }

    int readSaveFile(const std::string &filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile(yourgame_internal_android::saveFilesPath + filename, dst);
    }

    int readProjectFile(const std::string &filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile(yourgame_internal_android::projectPath + filename, dst);
    }

    void setProjectPath(const std::string &path)
    {
        yourgame_internal_android::projectPath = path;
        if (yourgame_internal_android::projectPath.back() != '/')
        {
            yourgame_internal_android::projectPath += "/";
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

        return yourgame_internal::readFile(filename, dst);
    }

    std::string getFileLocation(const std::string filepath)
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

    int writeSaveFile(const std::string &filename, const void *data, size_t numBytes)
    {
        return yourgame_internal::writeFile(
            yourgame_internal_android::saveFilesPath + filename, data, numBytes);
    }

    int writeProjectFile(const std::string &filename, const void *data, size_t numBytes)
    {
        return yourgame_internal::writeFile(
            yourgame_internal_android::projectPath + filename, data, numBytes);
    }

    std::vector<std::string> ls(const std::string &pattern)
    {
        // todo not implemented. dirent might work with android.
        std::vector<std::string> ret;
        return ret;
    }
} // namespace yourgame
