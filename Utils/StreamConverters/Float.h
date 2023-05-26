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