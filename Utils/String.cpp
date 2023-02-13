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
#include "Utils/String.h"
#include "Utils/Char.h"
#include "Utils/Definitions.h"

#define NOW std::chrono::high_resolution_clock::now().time_since_epoch().count()

namespace Rt2
{
    void StringUtils::replaceAll(String&       dest,
                                 const String& input,
                                 const String& a,
                                 const String& b)
    {
        if (a.empty())
        {
            dest = input;
            return;
        }

        OutputStringStream oss;
        for (size_t i = 0; i < input.size(); ++i)
        {
            if (const char ch = input[i]; ch != a[0])
                oss << ch;
            else
            {
                bool match = true;
                for (size_t j = 1; j < a.size() && match; ++j)
                {
                    if (i + j > input.size())
                        match = false;
                    else
                    {
                        const char aCh = input[i + j];
                        const char bCh = a[j];

                        match = aCh == bCh;
                    }
                }
                if (match)
                {
                    oss << b;
                    if (a.size() > 1)
                        i += a.size() - 1;
                }
                else
                    oss << ch;
            }
        }
        dest = oss.str();
    }

    void StringUtils::toLower(String& dest, const String& in)
    {
        OutputStringStream oss;
        for (const char& value : in)
            oss << (char)std::tolower(value);
        dest = oss.str();
    }

    void StringUtils::toUpper(String& dest, const String& in)
    {
        OutputStringStream oss;
        for (const char& value : in)
            oss << (char)std::toupper(value);
        dest = oss.str();
    }

    String StringUtils::toLowerFirst(const String& in)
    {
        String a = in;
        if (!a.empty())
            a[0] = (char)std::tolower((int)a[0]);
        return a;
    }

    String StringUtils::toUpperFirst(const String& in)
    {
        String a = in;
        if (!a.empty())
            a[0] = (char)std::toupper((int)a[0]);
        return a;
    }
    
    // clang-format off
    constexpr char BaseChars[] = {
        'D', 'c', 'U', 'y', '3', 'b', 'C', 'g',
        'p', 'J', '9', 'L', 'p', 'J', '0', 'A',
        'W', 'o', 'x', 'O', 'a', 'N', 'u', '8',
        'n', 'x', '7', 's', 'E', 'z', 'h', 'E',
        'q', 'Z', 'L', '4', 'F', '2', 'Q', 'i',
        'S', 'K', 'w', 'G', 'r', 'f', '1', 'Z',
        'd', 't', 'Y', '5', 'I', '3', 'K', 't',
        'B', '6', 'a', 'R', 'l', 'H', 'm', 'M',
    };
    // clang-format on

    constexpr size_t BaseCharsSize = sizeof BaseChars;

    void StringUtils::scramble(String& destination, size_t value, const bool randomize)
    {
        destination.clear();

        if (randomize)
            srand(NOW % 65536);

        while (value > 0)
        {
            const size_t q = value >> 6;

            size_t r;
            if (randomize)
            {
                const size_t ra = (size_t)rand();

                r = (value + ra) % BaseCharsSize;
            }
            else
                r = value % BaseCharsSize;
            destination.push_back(BaseChars[r]);
            value = q;
        }
    }

    union Merge
    {
        uint32_t i{0};
        uint8_t  b[8];

        explicit operator size_t() const
        {
            return i;
        }
    };

    String StringUtils::scramble(const String& value, bool randomize)
    {
        OutputStringStream oss;
        Merge              m;
        for (size_t i = 0; i < value.size(); ++i)
        {
            m.b[i % 8] = (uint8_t)value[i];
            if (i % 8 == 7)
            {
                String t;
                scramble(t, (size_t)m, randomize);
                oss << t;
            }
        }
        return oss.str();
    }

    void StringUtils::generate(String& destination, int& counter, void* seed)
    {
        // this 'should' be a GUID
        String sa, sb, sc, sd;
        scramble(sa, size_t(17) * ++counter);
        scramble(sb, NOW);
        scramble(sc, (size_t)seed);
        scramble(sd, size_t(41) * ++counter);
        join(destination, "L", sa, sb, sc, sd);
    }

    void StringUtils::splitLine(StringArray&  dest,
                                const String& input,
                                const char    swap)
    {
        String swap1, swap2;
        swap1.push_back(swap);
        swap2.push_back(swap);
        swap2.push_back(swap);

        String temp = input;
        replaceAll(temp, temp, "\r", swap1);
        replaceAll(temp, temp, "\n", swap1);
        replaceAll(temp, temp, swap2, "\r\n");
        replaceAll(temp, temp, swap1, "\r\n");
        split(dest, temp, "\r\n");
    }

    void StringUtils::split(StringArray&  destination,
                            const String& input,
                            const char    separator)
    {
        String inp;
        inp.push_back(separator);
        split(destination, input, inp);
    }

    void StringUtils::splitRejectEmpty(StringArray&  destination,
                                       const String& input,
                                       const char    separator)
    {
        String t0 = input;

        size_t pos = t0.find(separator);
        while (pos != String::npos)
        {
            String t1 = t0.substr(0, pos);
            trimWs(t1, t1);
            if (!t1.empty())
                destination.push_back(t1);

            t0  = t0.substr(pos + 1, t0.size());
            pos = t0.find(separator);
        }

        if (!t0.empty())
        {
            trimWs(t0, t0);
            destination.push_back(t0);
        }
    }

    void StringUtils::split(StringArray&  destination,
                            const String& input,
                            const String& separator)
    {
        String t0  = input;
        size_t pos = t0.find(separator);
        while (pos != String::npos)
        {
            destination.push_back(t0.substr(0, pos));
            t0  = t0.substr(pos + separator.size(), t0.size());
            pos = t0.find(separator);
        }

        if (!t0.empty())
            destination.push_back(t0);
    }

    void StringUtils::split(StringDeque&  destination,
                            const String& input,
                            const String& separator)
    {
        String tmp = input;

        size_t pos = tmp.find(separator);
        while (pos != String::npos)
        {
            destination.push_back(tmp.substr(0, pos));
            tmp = tmp.substr(pos + separator.size(), tmp.size());
            pos = tmp.find(separator);
        }

        if (!tmp.empty())
            destination.push_back(tmp);
    }

    void StringUtils::trim(String&       destination,
                           const String& input,
                           const char    character)
    {
        trimL(destination, input, character);
        trimR(destination, destination, character);
    }

    bool StringUtils::filterRange(
        String&       destination,
        const String& input,
        const int8_t  start,
        const int8_t  end,
        const size_t  max)
    {
        destination.clear();
        for (const char ch : input)
        {
            if (destination.size() >= max)
                break;

            if (ch >= start && ch <= end)
                destination.push_back(ch);
        }
        return destination.size() != input.size();
    }

    bool StringUtils::filterAZaz(String& destination, const String& input, const size_t max)
    {
        destination.clear();
        for (const char ch : input)
        {
            if (destination.size() >= max)
                break;

            if (isLetter(ch))
                destination.push_back(ch);
        }
        return destination.size() != input.size();
    }

    bool StringUtils::filterAscii(
        String&       destination,
        const String& input,
        const size_t  max)
    {
        destination.clear();
        for (const char ch : input)
        {
            if (destination.size() >= max)
                break;

            if (ch >= 0x20 && ch < 0x7F)
                destination.push_back(ch);
        }
        return destination.size() != input.size();
    }

    bool StringUtils::filterInt(
        String&       destination,
        const String& input,
        const size_t  max)
    {
        destination.clear();
        for (const char ch : input)
        {
            if (destination.size() >= max)
                break;

            if (ch >= '0' && ch < '9' || ch == '-')
                destination.push_back(ch);
        }
        return destination.size() != input.size();
    }

    bool StringUtils::filterReal(
        String&       destination,
        const String& input,
        const size_t  max)
    {
        destination.clear();
        for (const char ch : input)
        {
            if (destination.size() >= max)
                break;

            if (ch >= '0' && ch < '9' ||
                ch == '-' ||
                ch == '+' ||
                ch == 'E' ||
                ch == 'e' ||
                ch == 'F' ||
                ch == 'f' ||
                ch == '.')
                destination.push_back(ch);
        }
        return destination.size() != input.size();
    }

    void StringUtils::trimWs(String& di, const String& in)
    {
        const size_t n2 = in.size();
        const size_t n1 = (n2 >> 1) + n2 % 2;

        size_t i = 0;
        size_t x = Npos;
        size_t y = Npos;

        while (i < n1 && (x == Npos || y == Npos))
        {
            if (x == Npos)
            {
                if (const char f = in[i]; !isWs(f))
                    x = i;
            }
            if (y == Npos)
            {
                if (const char l = in[n2 - 1 - i]; !isWs(l))
                    y = n2 - i;
            }
            ++i;
        }

        if (x == Npos)
            x = 0;
        if (y == Npos)
            y = n2 > 1 ? n2 - 1 : 0;
        di = in.substr(x, y - x);
    }

    void StringUtils::trimL(String& destination, const String& input, const char character)
    {
        size_t st;
        for (st = 0; st < input.size(); ++st)
        {
            if (input[st] != character)
                break;
        }

        if (st < input.size())
            destination = input.substr(st, input.size());
        else
            destination.clear();
    }

    void StringUtils::trimR(String& destination, const String& input, const char character)
    {
        int en = (int)input.size() - 1;
        for (; en >= 0; --en)
        {
            if (input[en] != character)
            {
                ++en;
                break;
            }
        }

        if (en > 0)
            destination = input.substr(0, en);
        else
            destination.clear();
    }

    void StringUtils::trimRelPath(String& destination, const String& input)
    {
        destination.clear();

        bool fillState = false;
        for (const char ch : input)
        {
            if (fillState)
                destination.push_back(ch);
            else
            {
                fillState = ch != '/' && ch != '\\' && ch != '.';
                if (fillState)
                    destination.push_back(ch);
            }
        }
    }

}  // namespace Jam
