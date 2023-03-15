#pragma once
#include <iomanip>
#include "Utils/Array.h"
#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{

    template <typename T,
              uint8_t O,
              uint8_t S,
              uint8_t C,
              uint8_t W,
              uint8_t P,
              uint8_t F = 0>
    class TPrintSet : CallableStream<TPrintSet<T, O, S, C, W, P, F>>
    {
    public:
        using Set = Array<T>;

    private:
        Set     _data;
        uint8_t _w;
        uint8_t _p;
        bool    _fix{false};

    public:
        explicit TPrintSet(const Set&    data,
                           const uint8_t w   = W,
                           const uint8_t p   = P,
                           const bool    fix = F != 0) :
            _data(data), _w(w), _p(p), _fix(fix)
        {
        }

        OStream& operator()(OStream& out) const
        {
            if (O != '\0')
                out << (int8_t)O;

            out << std::setfill(' ');
            bool f = true;
            for (const auto& data : _data)
            {
                if (!f) out << (int8_t)S << ' ';

                out << std::setw(_w);
                out << std::setprecision(_p);
                if (_fix)
                    out << std::fixed;
                else
                    out << std::defaultfloat;
                out << data;
                f = false;
            }
            if (C != '\0')
                out << (int8_t)C;
            return out;
        }
    };

    using SetD      = TPrintSet<double, '[', ',', ']', DBL_DIG, DBL_DIG>;
    using SetF      = TPrintSet<float, '[', ',', ']', FLT_DIG, DBL_DIG>;
    using SetI      = TPrintSet<int32_t, '[', ',', ']', 0, 8>;
    using SetL      = TPrintSet<int64_t, '[', ',', ']', 0, 8>;
    using SetU      = TPrintSet<uint32_t, '[', ',', ']', 0, 8>;
    using SetUl     = TPrintSet<uint64_t, '[', ',', ']', 0, 8>;
    using SetS      = TPrintSet<String, '(', ',', ')', 0, 0>;
    using ValueSetF = TPrintSet<float, '\0', ',', '\0', 6, 3>;
    using ValueSetI = TPrintSet<int, '\0', ',', '\0', 0, 8>;
    using ValueSetU = TPrintSet<uint32_t, '\0', ',', '\0', 0, 8>;
    using ValueSetS = TPrintSet<String, '\0', ',', '\0', 0, 0>;
}  // namespace Rt2