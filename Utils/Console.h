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
#include <iomanip>
#include "Utils/String.h"

namespace Rt2
{
    enum ConsoleColor
    {
        CS_WHITE        = 0,
        CS_BLACK        = 30,
        CS_DARK_RED     = 31,
        CS_DARK_GREEN   = 32,
        CS_DARK_YELLOW  = 33,
        CS_DARK_BLUE    = 34,
        CS_DARK_MAGENTA = 35,
        CS_DARK_CYAN    = 36,
        CS_GREY         = 37,
        CS_LIGHT_GREY   = 90,
        CS_RED          = 91,
        CS_GREEN        = 92,
        CS_YELLOW       = 93,
        CS_BLUE         = 94,
        CS_MAGENTA      = 95,
        CS_CYAN         = 96,
    };

    class Console
    {
    public:
        enum Flags
        {
            NoColor     = 0x01,
            NoSequences = 0x02,
        };

    public:
        static void read();

        static void read(int8_t& v);
        static void read(uint8_t& v);
        static void read(int16_t& v);
        static void read(uint16_t& v);
        static void read(int32_t& v);
        static void read(uint32_t& v);
        static void read(int64_t& v);
        static void read(uint64_t& v);
        static void read(String& v);

        static void readln(String& v);

        static void setFlags(int flags);

        static void text(ConsoleColor col);
        static void background(ConsoleColor col);

        static void setForeground(ConsoleColor col);
        static void setBackground(ConsoleColor col);

        static void resetColor();

        [[deprecated]] static void write(IStream& stream);

        [[deprecated]] static void writeLine(const int64_t& i);
        [[deprecated]] static void writeLine(const double& i);

        static void write(const String& str);
        static void writeLine(const String& str);
        static void writeError(const String& str);

        static void hexdump(const String& str);
        static void hexdump(const OutputStringStream& str);
        static void hexdump(OStream& dest, const void* p, uint32_t len);
        static void hexdump(const void* p, uint32_t len);
        static void hexln(const void* p, uint32_t len);

        static void bindump(const String& str);
        static void bindump(const OutputStringStream& str);
        static void bindump(OStream& dest, const void* p, uint32_t len);
        static void bindump(const void* p, uint32_t len);

        template <typename... Args>
        [[deprecated]] static void writeLine(const String& str, Args&&... args)
        {
            OutputStringStream oss;
            oss << str;
            ((oss << std::forward<Args>(args)), ...);
            writeLine(oss.str());
        }

        template <typename... Args>
        [[deprecated]] static void writeError(const String& str, Args&&... args)
        {
            OutputStringStream oss;
            oss << str;
            ((oss << std::forward<Args>(args)), ...);
            writeError(oss.str());
        }

        template <typename... Args>
        [[deprecated]] static void write(const String& str, Args&&... args)
        {
            OutputStringStream oss;
            oss << str;
            ((oss << std::forward<Args>(args)), ...);
            write(oss.str());
        }

        [[deprecated]] static void writeLine(const OutputStringStream& str);
        [[deprecated]] static void writeError(const OutputStringStream& str);

        static void debugBreak();
        static void nl();

        template <typename... Args>
        static void println(Args&&... args)
        {
            OutputStringStream oss;
            ((oss << std::forward<Args>(args)), ...);
            writeLine(oss.str());
        }

        template <typename... Args>
        static void print(Args&&... args)
        {
            OutputStringStream oss;
            ((oss << std::forward<Args>(args)), ...);
            write(oss.str());
        }

        template <typename... Args>
        static void error(Args&&... args)
        {
            OutputStringStream oss;
            ((oss << std::forward<Args>(args)), ...);
            writeError(oss.str());
        }

        static void put(char c);

        static void puts(const String& str);

        static void puts(const char* str);

        static void clear();
        static void flush();

        static void execute(const String& exe, const OutputStringStream& args, String& dest);

        template <typename... Args>
        static String launch(const String& exe, Args&&... args)
        {
            OutputStringStream out;
            ((out << std::forward<Args>(args)), ...);

            String output;
            execute(exe, out, output);
            return output;
        }
    };

    using Out = Console;
    using Con = Console;
    using Dbg = Console;
}  // namespace Rt2