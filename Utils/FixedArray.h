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
#include "Utils/Exception.h"

namespace Rt2
{
    template <typename T, const uint16_t L>
    class FixedArray
    {
    public:
        using ValueType          = T;
        using ReferenceType      = T&;
        using PointerType        = T*;
        using ConstValueType     = const T;
        using ConstPointerType   = const T*;
        using ConstReferenceType = const T&;
        using SizeType           = uint16_t;

        static const uint16_t limit;

    protected:
        mutable ValueType _data[L + 1]{};
        uint16_t          _size{0};

    public:
        FixedArray() = default;

        FixedArray(const std::initializer_list<T>& o) :
            _size(Min<U16>(L, (U16)o.size()))
        {
            Fill(_data, data(o), _size);
        }

        void push_back(ConstReferenceType rhs)
        {
            if (_size < limit)
                _data[_size++] = rhs;
            else
                throw Exception("index out of bounds");
        }

        void pop_back()
        {
            if (_size > 0)
            {
                _data[_size-1].~T();
                --_size;
            }
        }

        uint16_t find(ConstReferenceType v)
        {
            for (uint16_t current = 0; current < _size; current++)
            {
                if (_data[current] == v)
                    return current;
            }

            return Npos16;
        }

        void remove(uint16_t pos)
        {
            if (_size > 0)
            {
                for (uint16_t i = pos + 1; i < _size; ++i)
                {
                    Swap(this->_data[i - 1],
                         this->_data[i]);
                }
                --_size;
            }
        }

        PointerType begin()
        {
            return _data;
        }

        PointerType end()
        {
            return _data + _size;
        }

        ConstPointerType begin() const
        {
            return _data;
        }

        ConstPointerType end() const
        {
            return _data + _size;
        }

        void resize(const uint16_t size)
        {
            if (size < limit)
            {
                if (size < _size)  // test
                {
                    for (uint16_t current = size; current < _size; current++)
                        _data[current].~T();
                    _size = size;
                }
                else
                {
                    while (_size < size)
                        _data[_size++] = T();
                }
            }
            else
                throw Exception("array limit exceeded");
        }

        void clear()
        {
            resize(0);
        }

        PointerType ptr()
        {
            return &_data[0];
        }

        ConstPointerType ptr() const
        {
            return &_data[0];
        }

        bool empty() const
        {
            return _size < 1;
        }

        uint16_t size() const
        {
            return _size;
        }

        ReferenceType back()
        {
            RT_ASSERT(!empty())
            return _data[_size - 1];
        }

        ConstReferenceType back() const
        {
            RT_ASSERT(!empty())
            return _data[_size - 1];
        }

        static uint16_t capacity()
        {
            return limit;
        }

        ConstReferenceType operator[](const uint16_t idx) const
        {
            return at(idx);
        }

        ConstReferenceType at(const uint16_t idx) const
        {
            return at(idx);
        }

        ReferenceType operator[](const uint16_t idx)
        {
            return at(idx);
        }

        ReferenceType at(const uint16_t idx)
        {
            if (idx < _size && idx < limit)
                return _data[idx];
            throw Exception("index out of bounds");
        }
    };

    template <typename T, const uint16_t L>
    const uint16_t FixedArray<T, L>::limit = L;

}  // namespace Rt2
