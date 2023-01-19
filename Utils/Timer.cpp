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
#include "Utils/Timer.h"
#include "Utils/Definitions.h"

#if RT_PLATFORM == RT_PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#include <ctime>
#endif

namespace Rt2
{
    Timer::Timer()
    {
        reset();
    }

    void Timer::reset()
    {
#if RT_PLATFORM == RT_PLATFORM_WINDOWS
        QueryPerformanceFrequency((LARGE_INTEGER*)&_private.freq);

        ULONG_PTR proc, sys;
        GetProcessAffinityMask(GetCurrentProcess(), (PDWORD_PTR)&proc, (PDWORD_PTR)&sys);
        proc          = Max<ULONG_PTR>(proc, 1);
        _private.mask = 1;

        while (_private.mask & proc)
            _private.mask <<= 1;

        QueryPerformanceCounter((LARGE_INTEGER*)&_private.begin);
        _private.current = 0;
#else
        gettimeofday(&_private.start, nullptr);
#endif
    }

    uint64_t Timer::getMilliseconds()
    {
#if RT_PLATFORM == RT_PLATFORM_WINDOWS

        const HANDLE thread = GetCurrentThread();

        const DWORD_PTR affinityMask = SetThreadAffinityMask(thread, _private.mask);

        QueryPerformanceCounter((LARGE_INTEGER*)&_private.current);

        SetThreadAffinityMask(thread, affinityMask);

        _private.current -= _private.begin;

        return static_cast<uint64_t>((double)_private.current * 1000 / (double)_private.freq);
#else
        gettimeofday(&_private.now, nullptr);
        return (_private.now.tv_sec - _private.start.tv_sec) * 1000 +
               (_private.now.tv_usec - _private.start.tv_usec) / 1000;

#endif
    }

    uint64_t Timer::getMicroseconds()
    {
#if RT_PLATFORM == RT_PLATFORM_WINDOWS
        const HANDLE    thread       = GetCurrentThread();
        const DWORD_PTR affinityMask = SetThreadAffinityMask(thread, _private.mask);

        QueryPerformanceCounter((LARGE_INTEGER*)&_private.current);

        SetThreadAffinityMask(thread, affinityMask);

        _private.current -= _private.begin;
        return static_cast<uint64_t>((double)_private.current * 1000000 / (double)_private.freq);

#else
        gettimeofday(&_private.now, nullptr);
        return (_private.now.tv_sec - _private.start.tv_sec) * 1000000 +
               (_private.now.tv_usec - _private.start.tv_usec);
#endif
    }

    uint64_t Timer::milliseconds()
    {
        static Timer timer;
        return timer.getMilliseconds();
    }

    uint64_t Timer::microseconds()
    {
        static Timer timer;
        return timer.getMicroseconds();
    }

    uint64_t Timer::tickCount()
    {
#if RT_PLATFORM == RT_PLATFORM_WINDOWS
        return ::GetTickCount64();
#else
        static timeval now;
        gettimeofday(&now, nullptr);
        return (now.tv_sec) / 1000;
#endif
    }

    void Timer::sleep(const uint32_t milliseconds)
    {
#if RT_PLATFORM == RT_PLATFORM_WINDOWS
        ::Sleep(milliseconds);
#else
        usleep(milliseconds);
#endif
    }

}  // namespace Rt2
