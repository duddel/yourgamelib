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
_POSIX_MONOTONIC_CLOCK should be defined (in unistd.h), if CLOCK_MONOTONIC is available.
not relying on that. instead, check if CLOCK_MONOTONIC works at run-time: checkMonotonic()
*/
#if (defined(__MINGW32__) || defined(__CYGWIN__) || defined(__linux__) || defined(__unix__))
#include <ctime>
#elif defined(_WIN32)
#include <windows.h>
#else
#error not implemented on this platform
#endif

#include <cstdint> // uint64_t
#include "yourgame_internal/timer.h"

namespace yourgame
{

namespace
{
bool checkMonotonic()
{
#if (defined(__MINGW32__) || defined(__CYGWIN__) || defined(__linux__) || defined(__unix__))
  struct timespec theTimeNow;
  return (clock_gettime(CLOCK_MONOTONIC, &theTimeNow) == 0);
#elif defined(_WIN32)
  return true;
#endif
}
} // namespace

Timer::Timer(uint64_t targetTicktimeUs) : _targetTicktime(targetTicktimeUs),
                                          _monotonic(checkMonotonic()),
                                          _timeLastTick(getNow()) {}

uint64_t Timer::tick()
{
  uint64_t timePassed;
  uint64_t timeNow;

  do
  {
    timeNow = getNow();
    // security check for non-monotonic clocks
    if (timeNow < _timeLastTick)
    {
      timePassed = 0U;
      break;
    }
    else
    {
      timePassed = (timeNow - _timeLastTick);
    }
  } while (timePassed < _targetTicktime);

  _timeLastTick = timeNow;
  return timePassed;
}

uint64_t Timer::getNow()
{
#if (defined(__MINGW32__) || defined(__CYGWIN__) || defined(__linux__) || defined(__unix__))
  if (_monotonic)
  {
    struct timespec theTimeNow;
    clock_gettime(CLOCK_MONOTONIC, &theTimeNow);
    return (uint64_t)(theTimeNow.tv_sec * 1000000 + (theTimeNow.tv_nsec / 1000));
  }
  else
  {
    // todo: implement fallback for non-monotonic POSIX clocks (gettimeofday()?)
    return 0U;
  }
#elif defined(_WIN32)
  LARGE_INTEGER perfTime, perfFreq;
  QueryPerformanceFrequency(&perfFreq);
  QueryPerformanceCounter(&perfTime);
  return (uint64_t)((perfTime.QuadPart * 1000000) / perfFreq.QuadPart);
#endif
}

} // namespace yourgame
