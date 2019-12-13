/*
Copyright (c) 2019 Alexander Scholz

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

/*
todo: this code is considered a stub: activity lifecycle, input, etc. not fully implemented.
*/
#include <android_native_app_glue.h>
#include "yourgame_internal/yourgame_port.h"

static void handleAppCmd(struct android_app *app, int32_t appCmd)
{
    switch (appCmd)
    {
    case APP_CMD_SAVE_STATE:
        break;
    case APP_CMD_INIT_WINDOW:
        yourgame::init(app);
        break;
    case APP_CMD_TERM_WINDOW:
        yourgame::shutdown();
        break;
    case APP_CMD_GAINED_FOCUS:
        break;
    case APP_CMD_LOST_FOCUS:
        break;
    }
}

void android_main(struct android_app *app)
{
    // register android app callbacks
    app->onAppCmd = handleAppCmd;

    while (true)
    {
        int ident;
        int outEvents;
        struct android_poll_source *outData;

        // poll all events
        while ((ident = ALooper_pollAll(0, NULL, &outEvents, (void **)&outData)) >= 0)
        {
            // process one event
            if (outData != NULL)
            {
                outData->process(app, outData);
            }

            // shutdown if requested
            if (app->destroyRequested != 0)
            {
                yourgame::shutdown();
                return;
            }
        }

        // tick engine
        yourgame::tick();
    }
}
