/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

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
-------------------------------------------------------------------------------
*/
#pragma once
#include <cstdint>
#include "Utils/Definitions.h"

#if RT2_PLATFORM == RT2_PLATFORM_WINDOWS
#else
#include <sys/time.h>
#include <unistd.h>
#endif

namespace Rt2
{
    class Timer
    {
    private:
#if RT2_PLATFORM == RT2_PLATFORM_WINDOWS
        struct TimerPrivate
        {
            TimerPrivate() :
                freq(0),
                begin(0),
                current(0),
                mask(1)
            {
            }

            uint64_t freq;
            uint64_t begin;
            uint64_t current;
            uint32_t mask;
        };

#else

        struct TimerPrivate
        {
            TimerPrivate() :
                start{},
                now{}
            {
            }

            struct timeval start;
            struct timeval now;
        };
#endif
        TimerPrivate _private;

    public:
        Timer();

        void reset();

        uint64_t getMilliseconds();
        uint64_t getMicroseconds();

        static uint64_t milliseconds();

        static uint64_t microseconds();

        static uint64_t tickCount();

        static void sleep(uint32_t milliseconds);
    };

}  // namespace Rt2
