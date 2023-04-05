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
#include "Utils/Array.h"
#include "Utils/Char.h"
#include "Utils/Definitions.h"
#include "Utils/Time.h"

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
                    if (a.size() > 1) i += a.size() - 1;
                }
                else
                    oss << ch;
            }
        }
        dest = oss.str();
    }

    void StringUtils::reverse(String& dest, const String& input)
    {
        if (input.size() > 1)
        {
            dest.clear();
            String::const_reverse_iterator it = input.rbegin();
            while (it != input.rend())
            {
                dest.push_back(*it);
                ++it;
            }
        }
        else
            dest = input;
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

    void StringUtils::copy(OStream&   out,
                           IStream&   in,
                           const bool binary,
                           const bool newLine)
    {
        if (binary)
        {
            in.seekg(0, std::ios::end);
            const std::streamsize sz = in.tellg();
            in.seekg(0, std::ios::beg);

            if (sz > 0)
            {
                String str;
                str.resize(sz);
                in.read(str.data(), sz);
                str[sz] = 0;

                out.write(str.data(), sz);
            }
        }
        else
        {
            in.seekg(0, std::ios::beg);
            while (!in.eof())
            {
                if (const int ch = in.get(); isPrintableAscii(ch))
                {
                    if (newLine || !isNewLine(ch))
                        out.put((char)ch);
                }
            }
        }
    }

    void StringUtils::copy(String& out, IStream& in, const bool newLine)
    {
        OutputStringStream oss;
        copy(oss, in, false, newLine);
        out.assign(oss.str());
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
            srand(Time::now16());

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

    String StringUtils::scramble(const String& value, const bool randomize)
    {
        OutputStringStream oss;

        Merge m{};
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
        scramble(sb, Time::now32());
        scramble(sc, (size_t)seed);
        scramble(sd, size_t(41) * ++counter);
        destination = join("L", sa, sb, sc, sd);
    }

    void StringUtils::cmd(String& dest, char** argv, const int argc, int from)
    {
        from = std::min<int>(from, argc);
        for (int i = from; i < argc; ++i)
        {
            if (i > from)
                dest.push_back(' ');
            dest.append(String(argv[i], Char::length(argv[i])));
        }
    }

    String StringUtils::csv(const StringArray& sa)
    {
        OutputStringStream oss;
        for (size_t i = 0; i < sa.size(); ++i)
        {
            oss << sa[i];
            if (i + 1 < sa.size())
                oss << ',';
        }
        return oss.str();
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

    void StringUtils::scanSplitLine(StringArray& dest, const String& input)
    {
        InputStringStream stream(input);
        SimpleArray<char> buf;
        buf.reserve(0x200);

        if (const size_t est = Clamp<size_t>(input.size() / 0x10, 0x00, 0x200);
            est > 0)
            dest.reserve(est);

        while (!stream.eof())
        {
            if (const int c0 = stream.get(); isNewLine(c0))
            {
                if (const int c1 = stream.peek(); c0 == '\r' && c1 == '\n')
                    stream.seekg(1, std::ios::cur);  // counts as 1

                if (buf.empty())
                    dest.push_back("");
                else
                {
                    dest.push_back("");
                    dest.back().assign(buf.data(), buf.size());
                    buf.resizeFast(0);
                }
            }
            else
            {
                if (isPrintableAscii(c0))
                    buf.push_back((char)c0);
                // else - filter it out..
            }
        }

        if (!buf.empty())
        {
            dest.push_back("");
            dest.back().assign(buf.data(), buf.size());
        }
    }

    void StringUtils::combine(String&            dest,
                              const StringDeque& input,
                              const char         in,
                              const char         out)
    {
        for (const auto& el : input)
        {
            if (in != 0)
                dest.push_back(in);
            dest.append(el);
            if (out != 0)
                dest.push_back(out);
        }
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

    String StringUtils::stripEol(const String& input, const char replacement)
    {
        OutputStringStream dest;
        StringArray        src;
        scanSplitLine(src, input);

        for (const auto& line : src)
        {
            String d1, d2;
            trim(d1, line, '\n');
            trim(d2, d1, '\r');
            trim(d2, d2, ' ');

            if (replacement != 0)
                d2.push_back(replacement);
            dest << d2;
        }
        return dest.str();
    }

    bool StringUtils::filter(String&               destination,
                             const String&         input,
                             const FilterFunction& pass,
                             const size_t          max)
    {
        if (!pass) return false;
        if (input.empty()) return false;

        if (!destination.empty())
            destination.clear();
        destination.reserve(input.size());

        for (const char ch : input)
        {
            if (destination.size() >= max) break;
            if (pass(ch)) destination.push_back(ch);
        }
        return destination.size() != input.size();
    }

    bool StringUtils::filterRange(
        String&       destination,
        const String& input,
        const int8_t  start,
        const int8_t  end,
        const size_t  max)
    {
        return filter(
            destination, input, [start, end](const char ch)
            { return ch >= start && ch <= end; },
            max);
    }

    bool StringUtils::filterAZaz(
        String&       destination,
        const String& input,
        const size_t  max)
    {
        return filter(destination, input, isLetter, max);
    }

    bool StringUtils::filterAZaz09(
        String&       destination,
        const String& input,
        const size_t  max)
    {
        return filter(destination, input, isAlphaNumeric, max);
    }

    bool StringUtils::filterAscii(
        String&       destination,
        const String& input,
        const size_t  max)
    {
        return filter(destination, input, isPrintableAscii, max);
    }

    bool StringUtils::filterInt(
        String&       destination,
        const String& input,
        const size_t  max)
    {
        return filter(destination, input, isInteger, max);
    }

    bool StringUtils::filterReal(
        String&       destination,
        const String& input,
        const size_t  max)
    {
        return filter(destination, input, isReal, max);
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

    bool StringUtils::startsWith(const String& test, const char& chk)
    {
        return !test.empty() && test.front() == chk;
    }

    bool StringUtils::startsWith(const String& test, const String& chk)
    {
        if (chk.empty())
            return test.empty();
        if (test.size() > chk.size())
            return test.substr(0, chk.size()) == chk;
        if (test.size() == chk.size())
            return test == chk;
        return false;
    }

    bool StringUtils::endsWith(const String& test, const char& chk)
    {
        return !test.empty() && test.back() == chk;
    }

    bool StringUtils::endsWith(const String& test, const String& chk)
    {
        if (chk.empty())
            return test.empty();
        if (test.size() > chk.size())
            return test.substr(test.size() - chk.size(), chk.size()) == chk;
        if (test.size() == chk.size())
            return test == chk;
        return false;
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

}  // namespace Rt2
