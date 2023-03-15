#pragma once
#include <iomanip>

#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{
    template <typename T>
    class Hex : CallableStream<Hex<T>>
    {
    private:
        size_t _value;

    public:
        explicit Hex(T v) :
            _value{(size_t)v} {}
        explicit Hex(T* v) :
            _value{(size_t)v} {}

        OStream& operator()(OStream& out) const
        {
            return out
                   << std::setfill('0')
                   << std::setw(sizeof(T) << 1)
                   << std::uppercase
                   << std::hex
                   << _value;
        }
    };
}  // namespace Rt2