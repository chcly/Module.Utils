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

#define RT_PLATFORM_UNIX 0
#define RT_PLATFORM_APPLE 1
#define RT_PLATFORM_WINDOWS 2
#define RT_PLATFORM_EMSCRIPTEN 3

#if defined(__EMSCRIPTEN__)
    #define RT_PLATFORM RT_PLATFORM_EMSCRIPTEN
#elif defined(_WIN32)
    #define RT_PLATFORM RT_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define RT_PLATFORM RT_PLATFORM_APPLE
#else
    #define RT_PLATFORM RT_PLATFORM_UNIX
#endif

#if RT_PLATFORM == RT_PLATFORM_UNIX
    #include <cstring>
    #include "stddef.h"
#endif

#if (defined(DEBUG) || defined(_DEBUG))
    #include "Utils/Assert.h"
    #define RT_DEBUG 1

    #define RT_ASSERT(x)                                               \
        {                                                              \
            if (!(x))                                                  \
                Rt2::AssertTrap(#x, __FILE__, __LINE__, __FUNCTION__); \
        }
    #define RT_DEBUG_CATCH()                    \
        catch (Rt2::Exception & ex)             \
        {                                       \
            Rt2::Console::writeLine(ex.what()); \
        }
#else
    #define RT_DEBUG_CATCH() \
        catch (...)          \
        {                    \
        }
    #define RT_ASSERT(x) ((void)(x));
    #define RT_DEBUG 0

#endif

#define RT_VOID_RET
#define RT_GUARD_MESSAGE_RELEASE

#ifdef RT_GUARD_MESSAGE_RELEASE
    #include "Utils/Assert.h"
    #define RT_GUARD_MESSAGE(PL) Rt2::AssertLog(#PL, __FILE__, __LINE__, __FUNCTION__)
#else
    #define RT_GUARD_MESSAGE(PL)
#endif


// Is a assert that has an exit path (opposed to a hard stop)
// (worst case redundancy)
#define RT_GUARD_CHECK_IMPL(PL, RET) \
    if (!(PL))                       \
    {                                \
        RT_GUARD_MESSAGE(PL);        \
        return RET;                  \
    }
#define RT_GUARD_CHECK_VOID(PL) RT_GUARD_CHECK_IMPL(PL, RT_VOID_RET)
#define RT_GUARD_CHECK_RET(PL, RV) RT_GUARD_CHECK_IMPL(PL, RV)

#define RT_COMPILER_MSVC 0
#define RT_COMPILER_GNU 1
#define RT_COMPILER_EMSCRIPTEN 2
#define RT_COMPILER_MSVC_CLANG 3

#if defined(__EMSCRIPTEN__)
    #define RT_COMPILER RT_COMPILER_EMSCRIPTEN
#elif defined(_MSC_VER)
    #if defined __clang__
        #define RT_COMPILER RT_COMPILER_MSVC
        #define RT_COMPILER_SUBTYPE RT_COMPILER_MSVC_CLANG
    #else
        #define RT_COMPILER RT_COMPILER_MSVC
        #define RT_COMPILER_SUBTYPE RT_COMPILER
    #endif
#elif defined(__GNUC__)
    #define RT_COMPILER RT_COMPILER_GNU
#else
    #error unknown compiler
#endif

#if RT_COMPILER == RT_COMPILER_MSVC
    #if RT_COMPILER_SUBTYPE == RT_COMPILER_MSVC_CLANG
        #define RT_FORCE_INLINE inline
    #else
        #define RT_FORCE_INLINE __forceinline
    #endif
#else
    #define RT_FORCE_INLINE inline
#endif

#define RT_ENDIAN_LITTLE 0
#define RT_ENDIAN_BIG 1

#if defined(__sgi) || defined(__sparc) ||     \
    defined(__sparc__) || defined(__PPC__) || \
    defined(__ppc__) || defined(__BIG_ENDIAN__)
    #define RT_ENDIAN RT_ENDIAN_BIG
#else
    #define RT_ENDIAN RT_ENDIAN_LITTLE
#endif

#define RT_ARCH_32 0
#define RT_ARCH_64 1

#if defined(__x86_64__) || defined(_M_X64) ||       \
    defined(__powerpc64__) || defined(__alpha__) || \
    defined(__ia64__) || defined(__s390__) ||       \
    defined(__s390x__)
    #define RT_ARCH RT_ARCH_64
#else
    #define RT_ARCH RT_ARCH_32
#endif

// #define RT_OPEN_MP 1
#ifdef RT_OPEN_MP
    #include <omp.h>
    #include <thread>
#endif

namespace Rt2
{
    using I8   = int8_t;
    using I16  = int16_t;
    using I32  = int32_t;
    using I64  = int64_t;
    using U8   = uint8_t;
    using U16  = uint16_t;
    using U32  = uint32_t;
    using U64  = uint64_t;
    using VInt = size_t;

    struct HandleT
    {
        size_t unused;
    };
    using Handle = HandleT*;

    template <typename T>
    constexpr const T& Max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }

    template <typename T>
    constexpr const T& Max3(const T& a, const T& b, const T& c)
    {
        return Max(a, Max(b, c));
    }

    template <typename T>
    constexpr const T& Min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template <typename T>
    void Swap(T& a, T& b)
    {
        T t(a);
        a = b;
        b = t;
    }

    template <typename T>
    constexpr const T& Min3(const T& a, const T& b, const T& c)
    {
        return Min(a, Min(b, c));
    }

    template <typename T>
    constexpr const T& Clamp(const T& v, const T& mi, const T& ma)
    {
        if (v > ma)
            return v < mi ? mi : ma;
        else
            return v < mi ? mi : v;
    }

    template <typename T>
    constexpr T Abs(const T& a)
    {
        return a < 0 ? -a : a;
    }

    template <typename T>
    constexpr T MakeLimit()
    {
        return (T)-1;
    }

    template <typename T>
    constexpr T MakeHalfLimit()
    {
        return MakeLimit<T>() >> 1;
    }

    constexpr size_t   Npos   = MakeLimit<size_t>();
    constexpr uint32_t Npos32 = MakeLimit<uint32_t>();
    constexpr uint32_t Npos16 = MakeLimit<uint16_t>();
    constexpr uint32_t Npos8  = MakeLimit<uint8_t>();

#ifdef RT_OPEN_MP
    template <typename T, size_t Ub = MakeLimit<size_t>()>
    void Fill(T* dest, const T* src, const size_t nr)
    {
        if (!(dest && src))
            return;
        if (const int64_t count = (int64_t)nr;
            count > 0 && nr < Ub)
        {
    #pragma omp parallel for num_threads(4)
            for (int64_t c = 0; c < count; ++c)
            {
                dest[c] = src[c];
            }
        }
    }

    template <typename T, size_t Ub = MakeLimit<size_t>()>
    void Fill(T* dest, const T& init, const size_t nr)
    {
        if (!dest)
            return;

        if (const int64_t count = (int64_t)nr;
            count > 0 && nr < Ub)
        {
    #pragma omp parallel for num_threads(4)
            for (int64_t c = 0; c < count; ++c)
            {
                dest[c] = init;
            }
        }
    }

#else

    template <typename Type, size_t UpperBound = MakeLimit<size_t>()>
    void Fill(Type* dest, const Type* src, const size_t nr)
    {
        if (nr < UpperBound && dest && src)
        {
            size_t i = 0;
            do
            {
                dest[i] = src[i];
            } while (++i < nr);
        }
    }

    template <typename Type, size_t UpperBound = MakeLimit<size_t>()>
    void Fill(Type* dest, const Type& src, const size_t nr)
    {
        if (nr < UpperBound && dest)
        {
            size_t i = 0;
            do
            {
                dest[i] = src;

            } while (++i < nr);
        }
    }
#endif
}  // namespace Rt2
