#pragma once
#include "Utils/String.h"

namespace Rt2
{
    template <typename T>
    struct CallableStream
    {
        friend OStream& operator<<(
            OStream& out,
            const T& set)
        {
            return set(out);
        }
    };
}  // namespace Rt2