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
#ifndef YOURGAME_YOURGAME_H
#define YOURGAME_YOURGAME_H

#include <cstdint> // uint64_t
#include <string>
#include "easylogging++.h"

namespace yourgame
{

struct context
{
    el::Logger *logger = NULL;
    double deltaTimeS = 0.0;
    uint64_t deltaTimeUs = 0U;
};

/** \brief returns the current context */
const yourgame::context &getCtx();

/** \brief triggers the application to exit after the cycle this function was called in
\attention no effect on platforms other than `desktop` */
void notifyShutdown();

/** \brief log debug */
template <typename T, typename... Args>
inline void logd(const char *s, const T &value, const Args &... args)
{
    el::Logger *logr = yourgame::getCtx().logger;
    if (logr != NULL)
        logr->debug(s, value, args...);
}
/** \brief log debug */
template <typename T>
inline void logd(const T &value)
{
    el::Logger *logr = yourgame::getCtx().logger;
    if (logr != NULL)
        logr->debug(value);
}

/** \brief log info */
template <typename T, typename... Args>
inline void logi(const char *s, const T &value, const Args &... args)
{
    el::Logger *logr = yourgame::getCtx().logger;
    if (logr != NULL)
        logr->info(s, value, args...);
}
/** \brief log info */
template <typename T>
inline void logi(const T &value)
{
    el::Logger *logr = yourgame::getCtx().logger;
    if (logr != NULL)
        logr->info(value);
}

/** \brief log warning */
template <typename T, typename... Args>
inline void logw(const char *s, const T &value, const Args &... args)
{
    el::Logger *logr = yourgame::getCtx().logger;
    if (logr != NULL)
        logr->warn(s, value, args...);
}
/** \brief log warning */
template <typename T>
inline void logw(const T &value)
{
    el::Logger *logr = yourgame::getCtx().logger;
    if (logr != NULL)
        logr->warn(value);
}

/** \brief log error */
template <typename T, typename... Args>
inline void loge(const char *s, const T &value, const Args &... args)
{
    el::Logger *logr = yourgame::getCtx().logger;
    if (logr != NULL)
        logr->error(s, value, args...);
}
/** \brief log error */
template <typename T>
inline void loge(const T &value)
{
    el::Logger *logr = yourgame::getCtx().logger;
    if (logr != NULL)
        logr->error(value);
}

} // namespace yourgame

#endif
