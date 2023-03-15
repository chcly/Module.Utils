#pragma once
#include <iomanip>
#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{
    template <uint8_t PadIn  = 1,
              uint8_t PadOut = 1>
    class Equ : CallableStream<Equ<PadIn, PadOut>>
    {
    private:
        uint8_t _pi;
        uint8_t _po;

    public:
        explicit Equ(const uint8_t pi = PadIn,
                     const uint8_t po = PadOut) :
            _pi(pi),
            _po(po)
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