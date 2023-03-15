#pragma once
#include <iomanip>
#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{
    struct Tab : CallableStream<Tab>
    {
        mutable uint8_t w{4};

        Tab() = default;

        explicit Tab(const uint8_t nw) :
            w(nw) {}

        OStream& operator()(OStream& out) const
        {
            w = std::max<uint8_t>(w, 1);
            return out
                   << std::setfill(' ')
                   << std::setw(std::streamsize(w) - 1)
                   << ' ';
        }
    };
}  // namespace Rt2