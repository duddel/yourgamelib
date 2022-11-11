/*
Copyright (c) 2019-2023 Alexander Scholz

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
#include <android_native_app_glue.h>
#include "yourgame_internal/yourgame_internal_android.h"
#include "yourgame/log.h"

static void handleAppCmd(struct android_app *app, int32_t appCmd)
{
    // logging works after yourgame_internal_android::init() was called
    // todo: split log::init() from init()?

    switch (appCmd)
    {
    case APP_CMD_INIT_WINDOW:
        yourgame_internal_android::init(app);
        yourgame::log::debug("APP_CMD_INIT_WINDOW");
        break;
    case APP_CMD_TERM_WINDOW:
        yourgame_internal_android::shutdown();
        yourgame::log::debug("APP_CMD_TERM_WINDOW");
        break;
    case APP_CMD_GAINED_FOCUS:
        yourgame::log::debug("APP_CMD_GAINED_FOCUS");
        break;
    case APP_CMD_LOST_FOCUS:
        yourgame::log::debug("APP_CMD_LOST_FOCUS");
        break;
    case APP_CMD_SAVE_STATE:
        yourgame::log::debug("APP_CMD_SAVE_STATE");
        break;
    case APP_CMD_DESTROY:
        yourgame::log::debug("APP_CMD_DESTROY");
        break;
    case APP_CMD_STOP:
        yourgame::log::debug("APP_CMD_STOP");
        break;
    }
}

void android_main(struct android_app *app)
{
    app->onAppCmd = handleAppCmd;

    while (true)
    {
        int ident;
        int outEvents;
        struct android_poll_source *outData;

        // poll all events. if the app is not visible, this loop blocks until yourgame_internal_android::isInitialized() returns true
        while ((ident = ALooper_pollAll(yourgame_internal_android::isInitialized() ? 0 : -1, NULL, &outEvents, (void **)&outData)) >= 0)
        {
            // process one event
            if (outData != NULL)
            {
                outData->process(app, outData);
            }

            // exit the app by returning from within the infinite loop
            if (app->destroyRequested != 0)
            {
                // yourgame_internal_android::shutdown() should have been called already while processing the
                // app command APP_CMD_TERM_WINDOW. but we play save here
                if (!yourgame_internal_android::isInitialized())
                {
                    yourgame_internal_android::shutdown();
                }
                return;
            }
        }

        // tick the engine
        yourgame_internal_android::tick();
    }
}
