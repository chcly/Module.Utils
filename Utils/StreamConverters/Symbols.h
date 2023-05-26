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