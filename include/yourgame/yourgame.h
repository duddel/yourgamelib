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
#ifndef YOURGAME_YOURGAME_H
#define YOURGAME_YOURGAME_H

#include <cstdint> // uint64_t
#include <string>
#include "easylogging++.h"

/* generates log function wrappers */
#define YOURGAME_LOG_FUNC(FUNC_NAME, EL_FUNC_NAME)                             \
    template <typename T, typename... Args>                                    \
    inline void FUNC_NAME(const char *s, const T &value, const Args &... args) \
    {                                                                          \
        el::Logger *logr = yourgame::getCtx().logger;                          \
        if (logr != NULL)                                                      \
            logr->EL_FUNC_NAME(s, value, args...);                             \
    }                                                                          \
    template <typename T>                                                      \
    inline void FUNC_NAME(const T &value)                                      \
    {                                                                          \
        el::Logger *logr = yourgame::getCtx().logger;                          \
        if (logr != NULL)                                                      \
            logr->EL_FUNC_NAME(value);                                         \
    }

namespace yourgame
{

struct context
{
    std::string assetPath;
    el::Logger *logger = NULL;
    double deltaTimeS = 0.0;
    uint64_t deltaTimeUs = 0U;
};

const yourgame::context &getCtx();

YOURGAME_LOG_FUNC(logd, debug)
YOURGAME_LOG_FUNC(logi, info)
YOURGAME_LOG_FUNC(logw, warn)
YOURGAME_LOG_FUNC(loge, error)

} // namespace yourgame

#undef YOURGAME_LOG_FUNC
#endif
