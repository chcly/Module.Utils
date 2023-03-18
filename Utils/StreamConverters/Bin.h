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