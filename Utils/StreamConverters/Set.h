#pragma once
#include <cfloat>
#include <iomanip>
#include "Utils/Array.h"
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{

    template <typename T,    // type
              uint8_t O,     // open symbol
              uint8_t S,     // separator symbol
              uint8_t C,     // close symbol
              uint8_t W,     // width
              uint8_t P,     // precision
              uint8_t F = 0  // fixed
              >
    class TPrintSet : OutOperator<TPrintSet<T, O, S, C, W, P, F>>
    {
    public:
        using Set = Array<T>;

    private:
        Set     _v;
        uint8_t _w;
        uint8_t _p;
        bool    _f{false};

    public:
        explicit TPrintSet(const Set&    d,
                           const uint8_t w   = W,
                           const uint8_t p   = P,
                           const bool    f = F != 0) :
            _v(d),
            _w(std::max<uint8_t>(w, 1) - 1),
            _p(p),
            _f(f)
        {
        }

        OStream& operator()(OStream& out) const
        {
            if (O != '\0') out << (int8_t)O;

            out << std::setfill(' ');

            bool f = true;
            for (const auto& data : _v)
            {
                if (!f) out << (int8_t)S << ' ';

                out << std::setw(_w);
                out << std::setprecision(_p);

                if (_f)
                    out << std::fixed;
                else
                    out << std::defaultfloat;

                out << data;
                f = false;
            }

            if (C != '\0') out << (int8_t)C;
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