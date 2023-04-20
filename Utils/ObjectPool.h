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
#include "Utils/Array.h"

namespace Rt2
{

    template <typename T, int Expansion = 0x40, int Opt = AOP_SIMPLE_TYPE>
    class ObjectPool
    {
    public:
        using PointerType = T*;
        using Pool        = Array<PointerType, Opt>;
        using SizeType    = typename Pool::SizeType;

    private:
        Pool _pool;

    public:
        ObjectPool()
        {
            static_assert(Expansion >= 0x01 && Expansion < 0x1000);
            reserve(Expansion);
        }

        ~ObjectPool()
        {
            clear();
        }

        void clear()
        {
            for (const auto el : _pool)
                delete el;

            if (Opt & AOP_SIMPLE_TYPE)
                _pool.resizeFast(0);
            else
                _pool.clear();
        }

        void reserve(const SizeType& space)
        {
            if (_pool.size() < space)
                _pool.reserve(space);

            SizeType st = _pool.size();
            for (SizeType i = st; i < capacity(); ++i)
                _pool.push_back(new T());
        }

        SizeType capacity() const
        {
            return _pool.capacity() - 1;
        }

        PointerType allocate()
        {
            if (!_pool.empty())
            {
                const PointerType val = _pool.back();
                _pool.pop_back();
                return val;
            }
            reserve(Expansion);
            return allocate();
        }

        void free(PointerType ptr)
        {
            if (ptr)
            {
                if (_pool.size() + 1 > _pool.capacity() - 1)
                    _pool.reserve(_pool.size() + Expansion);
                _pool.push_back(new (ptr) T());
            }
        }
    };
}  // namespace Rt2
