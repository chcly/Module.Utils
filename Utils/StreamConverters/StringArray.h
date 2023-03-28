#pragma once
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{
    class Sa : OutOperator<Sa>
    {
    private:
        StringArray _array;

    public:
        explicit Sa(StringArray sa) :
            _array(std::move(sa))
        {
        }

        OStream& operator()(OStream& out) const
        {
            out << Su::csv(_array);
            return out;
        }
    };

}  // namespace Rt2