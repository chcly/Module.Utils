/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
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

    class AttributeMap : OutOperator<AttributeMap>
    {
    private:
        String    _attr;
        StringMap _values;

    public:
        explicit AttributeMap(String attr, StringMap values) :
            _attr(std::move(attr)),
            _values(std::move(values))
        {
        }

        OStream& operator()(OStream& out) const
        {
            if (!_values.empty())
            {
                out << _attr << '=' << '"';
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