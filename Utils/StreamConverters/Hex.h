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
}  // namespace Rt2