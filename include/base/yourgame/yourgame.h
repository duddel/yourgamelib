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
#ifndef YOURGAME_YOURGAME_H
#define YOURGAME_YOURGAME_H

#include <cstdint> // uint64_t
// the following includes are part of the yourgame (base) API
#include "yourgame/fileio.h"
#include "yourgame/input.h"
#include "yourgame/logging.h"
#include "yourgame/version.h"

namespace yourgame
{
    struct context
    {
        double deltaTimeS = 0.0;
        uint64_t deltaTimeUs = 0U;
        uint32_t winWidth = 0U;
        uint32_t winHeight = 0U;
        float winAspectRatio = 1.0f;
        bool winIsFullscreen = false;
        bool vsyncEnabled = false;
        bool mouseCatched = false;
    };

    /** \brief returns the current context */
    yourgame::context getCtx();

    /** \brief returns (monotonic) time duration [seconds] since init */
    double getTime();

    /** \brief triggers the application to exit after the cycle this function was called in */
    void notifyShutdown();

    /** \brief sends a (user-defined) command to the application host environment */
    int sendCmdToEnv(int cmdId, int data0, int data1, int data2);

    /** \brief request fullscreen */
    void enableFullscreen(bool enable);

    /** \brief request vertical sync
    \attention if this works reliable depends on gpu/driver */
    void enableVSync(bool enable);

    /** \brief disable mouse cursor, infinite movement */
    void catchMouse(bool enable);
} // namespace yourgame

#endif
