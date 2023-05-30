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
#include "Char.h"
#include "Path.h"
#include "Utils/Definitions.h"
#ifdef _WIN32
    #include <Windows.h>
#endif

namespace Rt2
{
    static ConsoleColor gForeground = CS_WHITE;
    static ConsoleColor gBackground = CS_BLACK;

#ifndef _WIN32
    static uint8_t MapColor(const ConsoleColor& col, bool bg)
    {
        uint8_t color = 0;
        switch (col)
        {
        case CS_BLACK:
            color = 30;
            break;
        case CS_DARK_BLUE:
            color = 34;
            break;
        case CS_DARK_GREEN:
            color = 32;
            break;
        case CS_DARK_CYAN:
            color = 36;
            break;
        case CS_DARK_RED:
            color = 31;
            break;
        case CS_DARK_MAGENTA:
            color = 35;
            break;
        case CS_DARK_YELLOW:
            color = 33;
            break;
        case CS_LIGHT_GREY:
            color = 90;
            break;
        case CS_GREY:
            color = 37;
            break;
        case CS_BLUE:
            color = 94;
            break;
        case CS_GREEN:
            color = 92;
            break;
        case CS_CYAN:
            color = 96;
            break;
        case CS_RED:
            color = 91;
            break;
        case CS_MAGENTA:
            color = 95;
            break;
        case CS_YELLOW:
            color = 93;
            break;
        case CS_WHITE:
            color = 0;
            break;
        }
        if (bg)
            color += 10;
        return color;
    }

#endif

    static void SetColor()
    {
#ifdef _WIN32
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE),
            (UCHAR)((UCHAR)gBackground * 16 + (UCHAR)gForeground));
#else
        uint8_t color[2] = {
            MapColor(gForeground, false),
            MapColor(gBackground, true),
        };
        if (gBackground != CS_BLACK)
            printf("\033[%dm\033[%dm", color[0], color[1]);
        else
            printf("\033[%dm", color[0]);
#endif
    }

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

    void Console::readLine(String& v)
    {
        std::getline(std::cin, v);
    }

    void Console::setForeground(const ConsoleColor col)
    {
        gForeground = col;
    }

    void Console::setBackground(const ConsoleColor col)
    {
        gBackground = col;
    }

    void Console::resetColor()
    {
#ifndef _WIN32
        gForeground = CS_WHITE;
        gBackground = CS_BLACK;
        printf("\033[0m");
#else
        gForeground = CS_WHITE;
        gBackground = CS_BLACK;
        SetColor();
#endif
    }

    void Console::write(IStream& stream)
    {
        Su::copy(std::cout, stream, false);
    }

    void Console::write(const String& str)
    {
        SetColor();
        std::cout << str;

#if defined(_WIN32) && defined(_DEBUG)
        if (IsDebuggerPresent())
            OutputDebugString(str.c_str());
#endif
    }

    void Console::writeLine(const String& str)
    {
        SetColor();
        std::cout << str << std::endl;
        resetColor();
#if defined(_WIN32) && defined(_DEBUG)
        if (IsDebuggerPresent())
            OutputDebugString((str + "\n").c_str());
#endif
    }

    void Console::writeError(const String& str)
    {
        setForeground(CS_RED);
        SetColor();
        std::cerr << str << std::endl;
        resetColor();
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
        writeLine("");
    }

    void Console::put(char c)
    {
        std::cout.put(c);
    }

    void Console::execute(const String& exe, const OutputStringStream& args, String& dest)
    {
#ifdef _WIN32
    #define popen _popen
    #define pclose _pclose
#endif
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
            writeLine("Failed to launch: ", ss.str());
    }

// This meant to be switched off later
#define USE_EXCEPTION_BREAK
    void Console::debugBreak()
    {
#ifdef USE_EXCEPTION_BREAK
    #if defined(_WIN32) && defined(_DEBUG)

        if (IsDebuggerPresent())
            DebugBreak();
    #endif
#endif  // USE_EXCEPTION_BREAK
    }
}  // namespace Rt2
