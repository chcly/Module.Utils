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

#include "Utils/ArrayType.h"
#include "Utils/Traits.h"

namespace Rt2
{
    template <typename T, uint8_t Options = 0, typename Allocator = Allocator<T, uint32_t> >
    class Queue : public ArrayBase<T, Options, Allocator>
    {
    public:
        RT_DECLARE_TYPE(T)

        typedef Queue<T, Options, Allocator>     SelfType;
        typedef ArrayBase<T, Options, Allocator> BaseType;
        typedef typename SelfType::SizeType      SizeType;

    public:
        Queue()               = default;
        Queue(const Queue& q) = default;

        ~Queue()
        {
            clear();
        }

        void clear()
        {
            this->destroy();
        }

        void resize(SizeType nr)
        {
            this->resize(nr);
        }

        void enqueue(ConstReferenceType value)
        {
            if (this->_size + 1 > this->_alloc.limit)  // provide an upper limit
                return;

            if (this->_size + 1 > this->_capacity)
                this->reserve(this->_size == 0 ? 8 : this->_size * 2);

            this->_data[this->_size] = value;
            ++this->_size;
        }

        ReferenceType pop_front()
        {
            return dequeue();
        }

        ValueType dequeue()
        {
            if (this->_size < 1)
                throw Exception("dequeue on an empty queue");

            ValueType returnValue = this->_data[0];
            for (SizeType i = 0; i < this->_size - 1; ++i)
                Swap(this->_data[i], this->_data[i + 1]);

            --this->_size;
            return returnValue;
        }

        ReferenceType at(SizeType idx)
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(idx < this->_capacity);

            return (*this)[idx];
        }

        ConstReferenceType at(SizeType idx) const
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(idx < this->_capacity);
            return (*this)[idx];
        }

        ReferenceType operator[](SizeType idx)
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(idx < this->_capacity);
            return this->_data[idx];
        }

        ConstReferenceType operator[](SizeType idx) const
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(idx < this->_capacity);
            return this->_data[idx];
        }

        ConstReferenceType front() const
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(this->_size > 0);
            return this->_data[0];
        }

        ConstReferenceType back() const
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(this->_size > 0);
            return this->_data[this->_size - 1];
        }
    };

    template <typename T, uint8_t Options = 0, typename Allocator = Allocator<T, uint32_t> >
    class ShiftingQueue : public ArrayBase<T, Options, Allocator>
    {
    public:
        RT_DECLARE_TYPE(T)

        typedef Queue<T, Options, Allocator>     SelfType;
        typedef ArrayBase<T, Options, Allocator> BaseType;
        typedef typename SelfType::SizeType      SizeType;

    private:
        SizeType _first{};
        SizeType _last{};

        SizeType actualSize() const
        {
            RT_ASSERT(_first <= _last)
            return _last - _first;
        }

    public:
        ShiftingQueue()                       = default;
        ShiftingQueue(const ShiftingQueue& q) = default;

        ~ShiftingQueue()
        {
            clear();
        }

        void clear()
        {
            this->destroy();
        }

        void enqueue(ConstReferenceType value)
        {
            if (actualSize() > this->_alloc.limit)  // provide an upper limit
                return;

            if (_last + 1 > this->_capacity)
            {
                SizeType oldSize = actualSize();
                RT_ASSERT(_first + oldSize <= this->_capacity)

                if (oldSize < this->_capacity)
                {
                    for (size_t i=0; i<oldSize; ++i)
                        Swap(this->_data[i], this->_data[_first + i]);

                    _first = 0;
                    _last = oldSize;
                }
                else
                    this->reserve(this->_capacity == 0 ? 8 : this->_capacity * 2);
            }

            this->_data[_last] = value;
            ++_last;
            this->_size = actualSize();
        }

        ValueType dequeue()
        {
            RT_ASSERT(!this->empty())

            ValueType returnValue = this->_data[this->_first];
            ++_first;

            if (_first == _last)
            {
                _first = 0;
                _last  = 0;
            }

            this->_size = actualSize();
            return returnValue;
        }

        ReferenceType at(SizeType idx)
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(idx < this->_capacity);

            return (*this)[idx];
        }

        ConstReferenceType at(SizeType idx) const
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(idx < this->_capacity);
            return (*this)[idx];
        }

        ReferenceType operator[](SizeType idx)
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(idx < this->_capacity);
            return this->_data[idx];
        }

        ConstReferenceType operator[](SizeType idx) const
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(idx < this->_capacity);
            return this->_data[idx];
        }

        ConstReferenceType front() const
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(this->_size > 0);
            return this->_data[0];
        }

        ConstReferenceType back() const
        {
            RT_ASSERT(this->_data);
            RT_ASSERT(this->_size > 0);
            return this->_data[this->_last - 1];
        }

        ReferenceType pop_front()
        {
            return dequeue();
        }

        void push_back(ConstReferenceType value)
        {
            enqueue(value);
        }
    };

}  // namespace Rt2
