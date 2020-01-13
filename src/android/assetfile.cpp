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
#include <vector>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

namespace yourgame
{
namespace
{
AAssetManager *_assetManager;
}

void initAssetFile(struct android_app *app)
{
    _assetManager = app->activity->assetManager;
}

std::vector<uint8_t> readAssetFile(const char *filename)
{
    std::vector<uint8_t> buf{'\0'};
    AAsset *assDesc = AAssetManager_open(_assetManager,
                                         filename,
                                         AASSET_MODE_BUFFER);
    if (assDesc)
    {
        auto nBytes = AAsset_getLength(assDesc);
        buf.resize(nBytes);
        int64_t nBytesRead = AAsset_read(assDesc, buf.data(), buf.size());
        AAsset_close(assDesc);
    }
    // todo: check buf.size() against nBytesRead and handle error case
    return buf;
}

} // namespace yourgame
