#pragma once
#include <iomanip>
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{
    template <uint8_t PadIn  = 1,
              uint8_t PadOut = 1>
    class Equ : OutOperator<Equ<PadIn, PadOut>>
    {
    private:
        uint8_t _pi;
        uint8_t _po;

    public:
        explicit Equ(const uint8_t pi = PadIn,
                     const uint8_t po = PadOut) :
            _pi(std::max<uint8_t>(pi, 1) - 1),
            _po(std::max<uint8_t>(po, 1) - 1)
        {
        }

        OStream& operator()(OStream& out) const
        {
            out << std::setw(_pi) << ' ';
            out << ":=";
            return out << std::setw(_po) << ' ';
        }
    };

}  // namespace Rt2