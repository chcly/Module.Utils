#pragma once
#include <iomanip>
#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{
    template <typename T,
              uint8_t W = (sizeof(T) << 1) + 2,
              uint8_t P = sizeof(T) << 1>
    class FPrintT : CallableStream<FPrintT<T, W, P>>
    {
    private:
        T       _value;
        uint8_t _w;
        uint8_t _p;

    public:
        explicit FPrintT(const T&      v,
                         const uint8_t w = W,
                         const uint8_t p = P) :
            _value{v}, _w(w), _p(p) {}

        OStream& operator()(OStream& out) const
        {
            if (_p > 0)
                out << std::setprecision(_p) << std::fixed;
            else
                out << std::defaultfloat;
            return out << _value;
        }

        void from(IStream& in)
        {
            in >>
                std::setfill(' ') >>
                std::setw(_w) >>
                std::setprecision(_p) >>
                _value;
        }

        explicit operator T()
        {
            return _value;
        }
    };

    using PrintR32 = FPrintT<float, 5, 3>;
    using PrintR64 = FPrintT<double>;

}  // namespace Rt2