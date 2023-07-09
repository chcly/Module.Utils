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
#include <iomanip>
#include "Utils/Definitions.h"
#include "Utils/String.h"

namespace Rt2
{

    class WriteUtils
    {
    public:
        static void indent(OStream& out, int n)
        {
            n = (int)Min<size_t>(Max<size_t>(n, 0), 80);
            while (--n >= 0)
                out.put(' ');
        }

        [[deprecated]] static void newLine(OStream& out, int n)
        {
            n = Min(Max(n, 0), 20);
            while (--n >= 0)
                out << std::endl;
        }

        static void nl(OStream& out, int n)
        {
            n = Min(Max(n, 0), 20);
            while (--n >= 0)
                out << std::endl;
        }

        static void nl(OStream& out)
        {
            out << std::endl;
        }

        static void next(OStream& out, const int n)
        {
            nl(out);
            indent(out, n);
        }

        template <typename... Args>
        static void write(OStream& out, const int n, Args&&... args)
        {
            indent(out, n);
            ((out << std::forward<Args>(args)), ...);
            nl(out);
        }

        template <typename... Args>
        static void writeLine(OStream& out, const int n, const int line, Args&&... args)
        {
            indent(out, n);
            ((out << std::forward<Args>(args)), ...);
            nl(out, line);
        }

        template <typename... Args>
        static void print(OStream& out, Args&&... args)
        {
            ((out << std::forward<Args>(args)), ...);
        }

        template <typename... Args>
        static void println(OStream& out, Args&&... args)
        {
            ((out << std::forward<Args>(args)), ...);
            nl(out);
        }

        static void puts(OStream& out, const String& string)
        {
            out.write(string.c_str(), (std::streamsize)string.size());
        }

        static void puts(OStream& out, const char* string)
        {
            RT_GUARD_VOID(string)
            out.write(string, (std::streamsize)std::strlen(string));
        }

        static void put(OStream& out, const char& ch)
        {
            out.put(ch);
        }
    };

    using Ts = WriteUtils;
    using Ws = WriteUtils;

}  // namespace Rt2
