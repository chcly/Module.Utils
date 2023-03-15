#pragma once
#include <functional>
#include "Utils/Definitions.h"
#include "Utils/StreamConverters/Callable.h"

namespace Rt2
{

    template <typename T>
    bool isEmpty(const T& v)
    {
        return v.empty();
    }

    template <typename T>
    class Attribute : CallableStream<Attribute<T>>
    {
    private:
        using CheckFn = std::function<bool(const T&)>;

        String        _key;
        T             _val;
        const CheckFn _fn;

        bool empty() const
        {
            // conditionally write to
            // the stream if it's not empty
            return _fn ? _fn(_val) : true;
        }

    public:
        explicit Attribute(String key, T val, CheckFn cb = isEmpty<T>) :
            _key(std::move(key)),
            _val(std::move(val)),
            _fn(cb)
        {
        }

        OStream& operator()(OStream& out) const
        {
            if (empty())
                return out;
            return out << _key << '=' << '"' << _val << '"';
        }
    };

    class AttributeMap : CallableStream<AttributeMap>
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
                out << "style=" << '"';
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