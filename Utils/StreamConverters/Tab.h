#pragma once
#include <iomanip>
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{
    struct Tab : OutOperator<Tab>
    {
        int w{4};

        explicit Tab(const int nw) :
            w(Clamp<int>(nw, 0, 80)) {}

        OStream& operator()(OStream& out) const
        {
            int n = w;
            while (--n >= 0)
                out.put(' ');
            return out;
        }
    };
}  // namespace Rt2