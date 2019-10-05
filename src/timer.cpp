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
#if (defined(__MINGW32__) || defined(__CYGWIN__) || defined(__linux__) || defined(__unix__))
  #include <unistd.h> // POSIX macros
  #include <ctime>
  #if !(_POSIX_TIMERS && _POSIX_MONOTONIC_CLOCK)
    #error not implemented on this platform
  #endif
#elif defined(_WIN32)
  #include <windows.h>
#endif
#include <cstdint> // uint64_t
#include "yourgame/timer.h"

namespace yourgame
{

Timer::Timer(uint64_t targetTicktimeUs)
{
    _targetTicktime = targetTicktimeUs;
    _timeLastTick = getNow();
}

uint64_t Timer::tick()
{
    uint64_t timePassed;
    uint64_t timeNow;

    do
    {
        timeNow = getNow();
        if (timeNow < _timeLastTick)
        {
            timePassed = 0;
            break;
        }
        else
            timePassed = (timeNow - _timeLastTick);
    } while (timePassed < _targetTicktime);

    _timeLastTick = timeNow;
    return timePassed;
}

uint64_t Timer::getNow()
{
#if (defined(__MINGW32__) || defined(__CYGWIN__) || defined(__linux__) || defined(__unix__))
    struct timespec theTimeNow;
    clock_gettime(CLOCK_MONOTONIC, &theTimeNow);
    return (uint64_t)(theTimeNow.tv_sec * 1000000 + (theTimeNow.tv_nsec / 1000));
#elif defined(_WIN32)
    LARGE_INTEGER perfTime, perfFreq;
    QueryPerformanceFrequency(&perfFreq);
    QueryPerformanceCounter(&perfTime);
    return (uint64_t)((perfTime.QuadPart * 1000000) / perfFreq.QuadPart);
#endif
}

} // namespace yourgame
