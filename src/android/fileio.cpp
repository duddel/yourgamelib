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
#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include "yourgame_internal/file.h"

namespace yourgame_internal_android
{
    namespace
    {
        AAssetManager *assMan;
        std::string saveFilesPath = "";
    } // namespace

    void initFileIO(struct android_app *app)
    {
        assMan = app->activity->assetManager;
        saveFilesPath = std::string(app->activity->internalDataPath);
    }
} // namespace yourgame_internal_android

namespace yourgame
{
    int readAssetFile(const char *filename, std::vector<uint8_t> &dst)
    {
        AAsset *assDesc = AAssetManager_open(yourgame_internal_android::assMan,
                                             filename,
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

    int readSaveFile(const char *filename, std::vector<uint8_t> &dst)
    {
        return yourgame_internal::readFile((yourgame_internal_android::saveFilesPath + filename).c_str(), dst);
    }

    int writeSaveFile(const char *filename, const void *data, size_t numBytes)
    {
        return yourgame_internal::writeFile((yourgame_internal_android::saveFilesPath + filename).c_str(), data, numBytes);
    }

    void saveFilePath(const char *filename, std::string &dst)
    {
        dst = (yourgame_internal_android::saveFilesPath + filename);
    }
} // namespace yourgame
