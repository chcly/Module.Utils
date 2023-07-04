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
#include "Utils/Base64.h"
#include "Utils/Char.h"

namespace Rt2
{

    constexpr uint32_t MaxMask = 0b00111111;

    constexpr char SymbolSet[65] = {
        // clang-format off
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '-', '='
        // clang-format on
    };

    String Base64::encode(const String& inp)
    {
        String copy;
        encode(copy, inp);
        return copy;
    }

    void Base64::encode(String& dest, const String& inp)
    {
        uint8_t p[3]{0, 0, 0};
        int64_t n = (int64_t)inp.size();
        dest.reserve(uint32_t(4 * double(n) / 3));

        for (int64_t i = 0; i < n; i += 3)
        {
            p[2] = (uint8_t)inp[i];
            if (i + 1 > n)
                p[1] = 0;
            else
                p[1] = (uint8_t)inp[i + 1];

            if (i + 2 > n)
                p[0] = 0;
            else
                p[0] = (uint8_t)inp[i + 2];

            const int64_t cb = (int64_t)p[2] << 16 | (int64_t)p[1] << 8 | (int64_t)p[0];
            dest.push_back(SymbolSet[cb >> 18 & MaxMask]);
            dest.push_back(SymbolSet[cb >> 12 & MaxMask]);
            dest.push_back(SymbolSet[cb >> 6 & MaxMask]);
            dest.push_back(SymbolSet[cb & MaxMask]);
        }

        if (const int64_t r3 = 3 - n % 3;
            dest.size() >= 4 && r3 <= 2)
        {
            n = (int64_t)dest.size();
            for (int64_t i = 0; i < r3; ++i)
                dest[n - (i + 1)] = SymbolSet[64];
        }
    }

    String Base64::decode(const String& inp)
    {
        String copy;
        decode(copy, inp);
        return copy;
    }

    int decodeIndex(const char ch)
    {
        switch (ch)
        {
        case UpperCaseAz:
            return ch - 'A';
        case LowerCaseAz:
            return ch - 'a' + 26;
        case Digits09:
            return ch - '0' + 52;
        case '-':
        case '+':
        case '=':
            return 0;
        default:
            return -1;
        }
    }

    void Base64::decode(String& dest, const String& inp)
    {
        if (inp.size() % 4 > 0)
            return;

        const int64_t n = (int64_t)inp.size();

        for (int64_t i = 0; i < n; i += 4)
        {
            const int a = decodeIndex(inp[i]);
            const int b = decodeIndex(inp[i + 1]);
            const int c = decodeIndex(inp[i + 2]);
            const int d = decodeIndex(inp[i + 3]);

            const int64_t cb = (int64_t)a << 18 | (int64_t)b << 12 | (int64_t)c << 6 | (int64_t)d;
            const uint8_t ra = (uint8_t)((cb & 0xFF0000) >> 16);
            const uint8_t rb = (uint8_t)((cb & 0x00FF00) >> 8);
            const uint8_t rc = (uint8_t)(cb & 0x0000FF);

            if (ra > 0) dest.push_back((char)ra);
            if (rb > 0) dest.push_back((char)rb);
            if (rc > 0) dest.push_back((char)rc);
        }
    }
}  // namespace Rt2
