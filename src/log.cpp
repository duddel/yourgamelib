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
#ifdef YOURGAME_PLATFORM_ANDROID
#include <android/log.h>
#endif
#include "easylogging++.h"
#include "yourgame_internal/log.h"

INITIALIZE_EASYLOGGINGPP

namespace yourgame_internal
{
    namespace log
    {
        namespace
        {
#ifdef YOURGAME_PLATFORM_ANDROID
            class elAndroidDispatcher : public el::LogDispatchCallback
            {
            protected:
                void handle(const el::LogDispatchData *data) noexcept override
                {
                    auto logMsg = data->logMessage();
                    switch (logMsg->level())
                    {
                    case el::Level::Debug:
                        __android_log_print(ANDROID_LOG_DEBUG, "yourgame_app", "%s", logMsg->message().c_str());
                        break;
                    case el::Level::Info:
                        __android_log_print(ANDROID_LOG_INFO, "yourgame_app", "%s", logMsg->message().c_str());
                        break;
                    case el::Level::Warning:
                        __android_log_print(ANDROID_LOG_WARN, "yourgame_app", "%s", logMsg->message().c_str());
                        break;
                    case el::Level::Error:
                        __android_log_print(ANDROID_LOG_ERROR, "yourgame_app", "%s", logMsg->message().c_str());
                        break;
                    default:
                        break;
                    }
                }
            };
#endif
        }

        void init(int argc, char *argv[])
        {
            START_EASYLOGGINGPP(argc, argv);

#ifdef YOURGAME_PLATFORM_ANDROID
            el::Helpers::installLogDispatchCallback<elAndroidDispatcher>("AndroidDispatcher");
            el::Helpers::uninstallLogDispatchCallback<el::base::DefaultLogDispatchCallback>("DefaultLogDispatchCallback");
            elAndroidDispatcher *dispatcher = el::Helpers::logDispatchCallback<elAndroidDispatcher>("AndroidDispatcher");
            dispatcher->setEnabled(true);
#endif
        }
    }
} // namespace yourgame_internal
