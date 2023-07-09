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
#include "Utils/StackStream.h"

namespace Rt2
{
    class Log
    {
    private:
        static String* path;

        static void flush(const OutputStringStream& ss);

    public:
        static void open(const String& log);

        static void close();

        static void clear();

        static String current();

        static void endl(const int n = 1)
        {
            OutputStringStream oss;
            Ts::nl(oss, n);
            flush(oss);
        }

        template <typename... Args>
        static void print(Args&&... args)
        {
            OutputStringStream oss;
            Ts::print(oss, std::forward<Args>(args)...);
            flush(oss);
        }

        template <typename... Args>
        static void println(Args&&... args)
        {
            OutputStringStream oss;
            Ts::println(oss, std::forward<Args>(args)...);
            flush(oss);
        }

    };

}  // namespace Rt2
