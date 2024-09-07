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
#include <string>
#include <vector>
#include "dirent.h" // todo preparation for ls()
#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include "yourgame_internal/file.h"

namespace
{
    AAssetManager *assMan;
} // namespace

namespace yourgame_internal
{
    namespace file
    {
        namespace android
        {
            void initFile(struct android_app *app)
            {
                assMan = app->activity->assetManager;
            }
        } // namespace android
    } // namespace file
} // namespace yourgame_internal

namespace yourgame
{
    namespace file
    {
        std::string getAssetFilePath(const std::string &pathRelative)
        {
            return "";
        }

        std::string getBasePath(const std::string &pathRelative)
        {
            return "";
        }

        int readAssetFile(const std::string &filename, std::vector<uint8_t> &dst)
        {
            AAsset *assDesc = AAssetManager_open(assMan,
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

        int writeAssetFile(const std::string &filename, const void *data, size_t numBytes)
        {
            // assets are read-only on android
            return -1;
        }

        std::vector<std::string> ls(const std::string &pattern)
        {
            // todo not implemented. dirent might work with android.
            std::vector<std::string> ret;
            return ret;
        }
    } // namespace file
} // namespace yourgame
