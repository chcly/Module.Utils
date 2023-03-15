#pragma once
#include <iomanip>

#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{
    template <uint8_t W = 80, int8_t C = '-', int8_t R = ' '>
    class FillT : CallableStream<FillT<W, C, R>>
    {
    private:
        uint8_t _w;

    public:
        explicit FillT(const uint8_t w = W) :
            _w(w) {}

        OStream& operator()(OStream& out) const
        {
            out << std::setw(size_t(std::max((int)_w, 1) - 1));
            out << std::setfill((char)C) << (char)C;
            out << std::setfill((char)R);
            return out;
        }
    };

    using Line = FillT<32, '-', ' '>;

}  // namespace Rt2