#pragma once
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{
    struct Nl : OutOperator<Nl>
    {
        OStream& operator()(OStream& out) const
        {
            return out << std::endl;
        }
    };

    class NlTab : OutOperator<NlTab>
    {
    public:
        OStream& operator()(OStream& out) const
        {
            out << std::endl;
            return out << "    ";
        }
    };

}  // namespace Rt2