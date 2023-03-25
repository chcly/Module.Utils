#pragma once
#include <iomanip>
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{
    struct Tab : OutOperator<Tab>
    {
        uint8_t w{4};

        explicit Tab(const uint8_t nw) :
            w(std::max<uint8_t>(nw, 1) - 1) {}

        OStream& operator()(OStream& out) const
        {
            if (w == 0)
                return out;
            return out
                   << std::setfill(' ')
                   << std::setw(std::streamsize(w))
                   << ' ';
        }
    };
}  // namespace Rt2