#pragma once
#include <functional>
#include "Utils/Definitions.h"
#include "Utils/StreamConverters/OutOperator.h"

namespace Rt2
{

    template <typename T>
    bool isEmpty(const T& v)
    {
        return v.empty();
    }

    template <typename T>
    class Attribute : OutOperator<Attribute<T>>
    {
    private:
        using CheckFn = std::function<bool(const T&)>;

        String        _k;
        T             _v;
        const CheckFn _f;

        bool empty() const
        {
            // conditionally write to
            // the stream if it's not empty
            return _f ? _f(_v) : true;
        }

    public:
        explicit Attribute(String key, T val, CheckFn cb = isEmpty<T>) :
            _k(std::move(key)),
            _v(std::move(val)),
            _f(cb)
        {
        }

        OStream& operator()(OStream& out) const
        {
            if (empty())
                return out;
            return out << _k << '=' << '"' << _v << '"';
        }
    };

    class AttributeMap : OutOperator<AttributeMap>
    {
    private:
        StringMap _values;

    public:
        explicit AttributeMap(StringMap& values) :
            _values(std::move(values))
        {
        }

        OStream& operator()(OStream& out) const
        {
            if (!_values.empty())
            {
                out << "style=" << '"';  // This shouldn't be hard coded.
                for (const auto& [k, v] : _values)
                    out << k << ':' << v << ';';
                out << '"';
            }
            return out;
        }
    };

    using AttrI32    = Attribute<I32>;
    using AttrI16    = Attribute<I16>;
    using AttrI8     = Attribute<I8>;
    using AttrString = Attribute<String>;

}  // namespace Rt2