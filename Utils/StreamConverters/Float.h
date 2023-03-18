#pragma once
#include <iomanip>
#include <cfloat>
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{
    template <typename T,
              uint8_t W = sizeof(T) << 1,
              uint8_t P = sizeof(T) << 1>
    class FPrintT : OutOperator<FPrintT<T, W, P>>
    {
    private:
        T       _v;
        uint8_t _w;
        uint8_t _p;

    public:
        explicit FPrintT(const T&      v,
                         const uint8_t w = W,
                         const uint8_t p = P) :
            _v{v}, _w(w), _p(p) {}

        OStream& operator()(OStream& out) const
        {
            if (_p > 0)
                out << std::setprecision(_p) << std::fixed;
            else
                out << std::defaultfloat;
            return out << _v;
        }

        void from(IStream& in)
        {
            in >>
                std::setfill(' ') >>
                std::setw(_w) >>
                std::setprecision(_p) >>
                _v;
        }

        explicit operator T()
        {
            return _v;
        }
    };

    using PrintR32 = FPrintT<float, FLT_DIG, FLT_DIG>;
    using PrintR64 = FPrintT<double, DBL_DIG, DBL_DIG>;

}  // namespace Rt2