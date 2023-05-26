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
#include <bitset>
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{
    template <typename T, uint8_t Len, bool Pad = true>
    class Bin : OutOperator<Bin<T, Len, Pad>>
    {
    private:
        std::bitset<Len> _value;

    public:
        explicit Bin(const T& v) :
            _value(v) {}

        OStream& operator()(OStream& out) const
        {
            size_t        i   = 0;
            const String& str = _value.to_string();
            while (i < str.size())
            {
                out << str[i];
                if (Pad)
                {
                    if (i + 1 < str.size() && i % 4 == 3)
                        out << "`";
                }
                ++i;
            }
            return out;
        }
    };

    using Bin8  = Bin<uint8_t, 8>;
    using Bin16 = Bin<uint16_t, 16>;
    using Bin32 = Bin<uint32_t, 32>;
    using Bin64 = Bin<uint64_t, 64>;
}  // namespace Rt2