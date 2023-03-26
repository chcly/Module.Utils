#pragma once
#include <iomanip>
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{
    template <uint8_t W = 80, int8_t C = '-', int8_t R = ' '>
    class FillT : OutOperator<FillT<W, C, R>>
    {
    private:
        std::streamsize _w;

    public:
        explicit FillT(const uint8_t w = W)
        {
            _w = std::max<std::streamsize>((std::streamsize)w, 1) - 1;
        }

        OStream& operator()(OStream& out) const
        {
            out << std::setw(_w);
            out << std::setfill((char)C) << (char)C;
            out << std::setfill((char)R);
            return out;
        }
    };

    using Line = FillT<32, '-', ' '>;

}  // namespace Rt2