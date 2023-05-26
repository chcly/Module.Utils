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
    template <typename T>
    class Hex : OutOperator<Hex<T>>
    {
    private:
        size_t _v;

    public:
        explicit Hex(T v) :
            _v{(size_t)v} {}
        explicit Hex(T* v) :
            _v{(size_t)v} {}

        OStream& operator()(OStream& out) const
        {
            return out
                   << std::setfill('0')
                   << std::setw(sizeof(T) << 1)
                   << std::uppercase
                   << std::hex
                   << _v;
        }
    };


    template <typename T>
    class HexConstant : OutOperator<HexConstant<T>>
    {
    private:
        size_t _v;

    public:
        explicit HexConstant(T v) :
            _v{(size_t)v} {}
        explicit HexConstant(T* v) :
            _v{(size_t)v} {}

        OStream& operator()(OStream& out) const
        {
            return out << '0' << 'x'
                   << std::setfill('0')
                   << std::setw(sizeof(T) << 1)
                   << std::uppercase
                   << std::hex
                   << _v;
        }
    };

}  // namespace Rt2