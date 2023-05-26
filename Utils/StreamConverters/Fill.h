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