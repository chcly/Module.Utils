#pragma once
#include <iomanip>
#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{
    class HexId : CallableStream<HexId>
    {
    public:
        OStream& operator()(OStream& out) const
        {
            static int nr = 0;

            return out << std::setfill('0')
                       << std::setw(8)
                       << std::uppercase
                       << std::hex
                       << ++nr;
        }
    };
}  // namespace Rt2