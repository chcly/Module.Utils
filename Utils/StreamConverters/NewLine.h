#pragma once
#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{
    struct Nl : CallableStream<Nl>
    {
        OStream& operator()(OStream& out) const
        {
            return out << std::endl;
        }
    };

    class NlTab : CallableStream<NlTab>
    {
    public:
        OStream& operator()(OStream& out) const
        {
            out << std::endl;
            return out << "    ";
        }
    };

}  // namespace Rt2