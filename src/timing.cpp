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
#include <chrono>
#include "yourgame_internal/timing.h"

namespace yourgame_internal
{
    namespace
    {
        double clockPeriod = 0.0;
        double deltaTimeS = 0.0;
        std::chrono::steady_clock::time_point lastNowTime;
        std::chrono::steady_clock::time_point initTime;
    }

    void initTiming()
    {
        clockPeriod = (double)std::chrono::steady_clock::period::num /
                      (double)std::chrono::steady_clock::period::den;
        lastNowTime = std::chrono::steady_clock::now();
        initTime = lastNowTime;
    }

    void tickTiming()
    {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = now - lastNowTime;
        lastNowTime = now;
        deltaTimeS = (duration.count());
    }
} // namespace yourgame_internal

namespace yourgame
{
    double getClockPeriod()
    {
        return yourgame_internal::clockPeriod;
    }

    double timeDelta()
    {
        return yourgame_internal::deltaTimeS;
    }

    double measureTime()
    {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = now - yourgame_internal::initTime;
        return duration.count();
    }
} // namespace yourgame
