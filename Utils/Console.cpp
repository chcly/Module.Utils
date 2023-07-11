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
#include "Utils/Console.h"
#include <bitset>
#include <iostream>
#include <sstream>
#include "FixedString.h"
#include "Utils/Char.h"
#include "Utils/Path.h"
#include "Utils/TextStreamWriter.h"
#ifdef _WIN32
    #include <Windows.h>
    #define popen _popen
    #define pclose _pclose
    #ifdef _DEBUG
        #define USE_EXCEPTION_BREAK  // This meant to be switched off later
        #define Break()              \
            if (IsDebuggerPresent()) \
            {                        \
                DebugBreak();        \
            }

        #define LogImmediateWindow(msg) \
            if (IsDebuggerPresent())    \
            {                           \
                OutputDebugString(msg); \
            }
    #else
        #define LogImmediateWindow(msg)
        #define Break()
    #endif
#else
    #define LogImmediateWindow(msg)
    #define Break()
#endif

namespace Rt2
{
    namespace Detail
    {
        constexpr const char* Clear      = "\x1b[2J";
        constexpr const char* ZeroCursor = "\x1b[0;0H";
        constexpr const char* ResetColor = "\x1b[0m";
        constexpr const char* Sequence   = "\x1b[";

        using TempBuf = char[32];

        class Private
        {
        private:
            static uint8_t fg;
            static uint8_t bg;
            static int     flags;
            static TempBuf buf;

        public:
            static void setForeground(ConsoleColor col);

            static void setBackground(ConsoleColor col);

            static void applyColor();

            static void clear();

            static void put(char ch);

            static void nl();

            static void write(const String& line, bool newline = false);

            static void error(const String& line, bool newline = false);

            static void resetColor();

            static void setFlags(int fl);

        private:
            static void clear(OStream& dest);

            static void applyColor(OStream& dest);

            static void resetColor(OStream& dest);

            static void nl(OStream& dest);

            static void write(OStream& dest, const String& line, bool newline = false);
        };

        uint8_t Private::fg = CS_WHITE;
        uint8_t Private::bg = CS_BLACK;

#if Utils_NO_COLOR == 1
        int Private::flags = Console::NoSequences | Console::NoColor;
#else
        int Private::flags = 0;
#endif
        TempBuf Private::buf = {};

    }  // namespace Detail

    void Console::read()
    {
        std::cin.get();
    }

    void Console::read(int8_t& v)
    {
        std::cin >> v;
    }

    void Console::read(uint8_t& v)
    {
        std::cin >> v;
    }

    void Console::read(int16_t& v)
    {
        std::cin >> v;
    }

    void Console::read(uint16_t& v)
    {
        std::cin >> v;
    }

    void Console::read(int32_t& v)
    {
        std::cin >> v;
    }

    void Console::read(uint32_t& v)
    {
        std::cin >> v;
    }

    void Console::read(int64_t& v)
    {
        std::cin >> v;
    }

    void Console::read(uint64_t& v)
    {
        std::cin >> v;
    }

    void Console::read(String& v)
    {
        std::cin >> v;
    }

    void Console::readln(String& v)
    {
        std::getline(std::cin, v);
        std::cin.clear();
    }

    void Console::setFlags(const int flags)
    {
        Detail::Private::setFlags(flags);
    }

    void Console::text(const ConsoleColor col)
    {
        setForeground(col);
    }

    void Console::background(const ConsoleColor col)
    {
        setBackground(col);
    }

    void Console::setForeground(const ConsoleColor col)
    {
        Detail::Private::setForeground(col);
    }

    void Console::setBackground(const ConsoleColor col)
    {
        Detail::Private::setBackground(col);
    }

    void Console::resetColor()
    {
        Detail::Private::resetColor();
    }

    void Console::write(IStream& stream)
    {
        Su::copy(std::cout, stream, false);
    }

    void Console::write(const String& str)
    {
        Detail::Private::write(str);
    }

    void Console::writeLine(const String& str)
    {
        Detail::Private::write(str, true);
    }

    void Console::writeError(const String& str)
    {
        Detail::Private::error(str);
    }

    void printHexLine(OStream&     dest,
                      const char*  buffer,
                      const size_t offs,
                      const size_t max)
    {
        if (!buffer)
            return;

        size_t        i, j;
        unsigned char ch;

        for (i = 0x00; i < 0x10; ++i)
        {
            j = i + offs;
            if (i % 0x09 == 0x08)
                dest << ' ';
            if (j >= max)
                dest << "00 ";
            else
            {
                ch = buffer[j];
                dest << std::setfill('0')
                     << std::setw(2)
                     << std::uppercase
                     << std::hex << (unsigned int)ch
                     << ' ';
            }
        }

        dest << '|';
        for (i = 0x00; i < 0x10; ++i)
        {
            j = i + offs;
            if (j >= max)
                dest << '.';
            else
            {
                ch = buffer[j];
                if (ch >= 0x20 && ch < 0x7F)
                    dest << (char)ch;
                else
                    dest << '.';
            }
        }
        dest << '|';
        dest << '\n';
    }

    void printHex(OStream& dest, const char* buffer, const uint32_t len)
    {
        if (buffer)
        {
            for (size_t i = 0; i < len; i += 0x10)
                printHexLine(dest, buffer, i, len);
        }
    }

    void printHex(const char* buffer, const uint32_t len)
    {
        printHex(std::cout, buffer, len);
    }

    void Console::hexdump(const String& str)
    {
        printHex(str.c_str(), (uint32_t)str.size());
    }

    void Console::hexdump(const OutputStringStream& str)
    {
        hexdump(str.str());
    }

    void Console::hexdump(
        OStream&       dest,
        const void*    p,
        const uint32_t len)
    {
        if (p)
            printHex(dest, (const char*)p, len);
    }

    void Console::hexdump(const void* p, const uint32_t len)
    {
        if (p)
            hexdump(std::cout, p, len);
    }

    void Console::hexln(const void* p, const uint32_t len)
    {
        if (!p) return;

        const char* ch = (const char*)p;
        for (uint32_t i = 0; i < len; ++i)
            std::cout << std::hex << (int)(uint8_t)ch[i];
        std::cout << std::endl;
    }

    void printBinLine(OStream&     dest,
                      const char*  buffer,
                      const size_t offs,
                      const size_t max)
    {
        if (!buffer)
            return;

        size_t        i, j;
        unsigned char ch;

        for (i = 0x00; i < 0x8; ++i)
        {
            j = i + offs;
            if (i % 0x05 == 0x04)
                dest << ' ';
            if (j >= max)
                dest << "0000 ";
            else
            {
                ch = buffer[j];
                std::bitset<4> set(ch);
                dest << set.to_string()
                     << ' ';
            }
        }

        dest << '|';
        for (i = 0x00; i < 0x8; ++i)
        {
            j = i + offs;
            if (j >= max)
                dest << '.';
            else
            {
                ch = buffer[j];
                if (ch >= 0x20 && ch < 0x7F)
                    dest << (char)ch;
                else
                    dest << '.';
            }
        }
        dest << '|';
        dest << '\n';
    }

    void printBin(OStream& dest, const char* buffer, const uint32_t len)
    {
        if (buffer)
        {
            for (size_t i = 0; i < len; i += 0x8)
                printBinLine(dest, buffer, i, len);
        }
    }

    void Console::bindump(const String& str)
    {
        bindump(str.c_str(), (uint32_t)str.size());
    }

    void Console::bindump(const OutputStringStream& str)
    {
        bindump(str.str());
    }

    void Console::bindump(
        OStream&       dest,
        const void*    p,
        const uint32_t len)
    {
        if (p)
            printBin(dest, (const char*)p, len);
    }

    void Console::bindump(const void* p, const uint32_t len)
    {
        if (p)
            bindump(std::cout, p, len);
    }

    void Console::writeLine(const int64_t& i)
    {
        OutputStringStream oss;
        oss << i;
        writeLine(oss);
    }

    void Console::writeLine(const double& i)
    {
        OutputStringStream oss;
        oss << i;
        writeLine(oss);
    }

    void Console::writeLine(const OutputStringStream& str)
    {
        writeLine(str.str());
    }

    void Console::writeError(const OutputStringStream& str)
    {
        writeError(str.str());
    }

    void Console::nl()
    {
        Detail::Private::nl();
    }

    void Console::put(const char c)
    {
        Detail::Private::put(c);
    }

    void Console::puts(const String& str)
    {
        Detail::Private::write(str, true);
    }

    void Console::puts(const char* str)
    {
        Detail::Private::write(str, true);
    }

    void Console::clear()
    {
        Detail::Private::clear();
    }

    void Console::flush()
    {
        std::cout.flush();
    }

    void Console::execute(const String& exe, const OutputStringStream& args, String& dest)
    {
        PathUtil prgDir(exe);

#ifdef _WIN32
        if (!prgDir.hasExtensions())
            prgDir = PathUtil(exe + ".exe");
#endif
        if (!prgDir.exists() || !prgDir.isAbsolute())
            return;

        StringStream ss;
        ss << prgDir.fullPath() << ' ' << args.str();

        if (FILE* fp = popen(ss.str().c_str(), "r"))
        {
            StringStream out;
            char         buf[128]{};
            char         ascii[128]{};
            while (!feof(fp))
            {
                if (size_t br = fread(buf, 1, 127, fp);
                    br < 128)
                {
                    size_t n = 0;
                    for (size_t i = 0; i < br; ++i)
                    {
                        if (isPrintableAscii(buf[i]))
                        {
                            ascii[n++] = buf[i];
                            ascii[n]   = 0;
                        }
                    }
                    buf[br] = 0;
                    if (n > 0)
                        out << ascii;
                }
            }
            pclose(fp);
            dest = out.str();
        }
        else
            error("Failed to launch: ", ss.str());
    }

    void Console::debugBreak()
    {
        Break();
    }

    namespace Detail
    {
        void Private::setForeground(const ConsoleColor col)
        {
            RT_GUARD_VOID((flags & Console::NoColor) == 0)
            fg = col;
        }

        void Private::setBackground(const ConsoleColor col)
        {
            RT_GUARD_VOID((flags & Console::NoColor) == 0)
            bg = col;
        }

        void Private::applyColor()
        {
            RT_GUARD_VOID((flags & Console::NoColor) == 0)
            applyColor(std::cout);
        }

        void Private::clear()
        {
            clear(std::cout);
            std::cin.clear();
        }

        void Private::put(const char ch)
        {
            buf[0] = ch;
            buf[1] = 0;
            write(std::cout, buf, false);
        }

        void Private::nl()
        {
            nl(std::cout);
        }

        void Private::write(const String& line, const bool newline)
        {
            write(std::cout, line, newline);
        }

        void Private::error(const String& line, const bool newline)
        {
            setForeground(CS_RED);
            write(std::cerr, line, newline);

            if (!newline)  // force  a reset here
                resetColor(std::cerr);
        }

        void Private::resetColor()
        {
            resetColor(std::cout);
        }

        void Private::setFlags(const int fl)
        {
            flags = fl;
        }

        void Private::clear(OStream& dest)
        {
            RT_GUARD_VOID((flags & Console::NoSequences) == 0)
            Ts::puts(dest, ZeroCursor);
            Ts::puts(dest, Clear);
        }

        void Private::applyColor(OStream& dest)
        {
            RT_GUARD_VOID((flags & Console::NoSequences) == 0)
            RT_GUARD_VOID((flags & Console::NoColor) == 0)

            (void)std::snprintf(buf, 4, "%d", (int)fg);
            Ts::print(dest, Sequence, buf, 'm');
            if (bg != CS_BLACK)  // use the default background
            {
                (void)std::snprintf(buf, 4, "%d", bg + 10);
                Ts::print(dest, Sequence, buf, 'm');
            }
        }

        void Private::resetColor(OStream& dest)
        {
            RT_GUARD_VOID((flags & Console::NoColor) == 0)
            fg = CS_WHITE;
            bg = CS_BLACK;
            Ts::puts(dest, ResetColor);
        }

        void Private::nl(OStream& dest)
        {
            LogImmediateWindow("\n");
            Ts::nl(dest);
            resetColor(dest);
        }

        void Private::write(OStream& dest, const String& line, const bool newline)
        {
            LogImmediateWindow(line.c_str());
            applyColor(dest);
            if (newline)
            {
                Ts::print(dest, line);
                nl(dest);
            }
            else
                Ts::print(dest, line);
        }

    }  // namespace Detail

}  // namespace Rt2
