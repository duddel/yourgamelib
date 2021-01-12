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
#ifndef YOURGAME_INTERNAL_ANDROID_H
#define YOURGAME_INTERNAL_ANDROID_H

#include <android_native_app_glue.h>

namespace yourgame_internal_android
{
    bool isInitialized();
    int32_t handleInputEvent(AInputEvent *inputEvent);
    void init(struct android_app *app);
    void tick();
    void shutdown();
    void initFileIO(struct android_app *app);
} // namespace yourgame

#endif
