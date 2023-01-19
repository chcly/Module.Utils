/*
-------------------------------------------------------------------------------

    Copyright (c) Charles Carley.

    Contributor(s): none yet.

-------------------------------------------------------------------------------
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

#include "HashMap.h"
#include "Utils/Allocator.h"
#include "Utils/Hash.h"

namespace Rt2
{
    template <typename T, typename Allocator = Allocator<Entry<T, bool> > >
    class Set
    {
    public:
        using TableType = HashTable<T, bool, Allocator>;

        RT_DECLARE_REF_TYPE(TableType)

        using SelfType = Set<T, Allocator>;

    public:
        Set()  = default;
        ~Set() = default;

        Set(const Set& oth) :
            _table(oth)
        {
        }

        void clear()
        {
            _table.clear();
        }

        bool insert(const T& v)
        {
            return _table.insert(v, true);
        }

        void erase(const T& v)
        {
            _table.remove(v);
        }

        size_t find(const T& v)
        {
            return _table.find(v);
        }

        T& operator[](size_t idx)
        {
            RT_ASSERT(idx < size());
            return _table.at(idx);
        }

        const T& operator[](size_t idx) const
        {
            RT_ASSERT(idx < size());
            return _table.at(idx);
        }

        T& at(size_t idx)
        {
            RT_ASSERT(idx < size());
            return _table.at(idx);
        }

        const T& at(size_t idx) const
        {
            RT_ASSERT(idx < size());
            return _table.at(idx);
        }

        size_t size() const
        {
            return _table.size();
        }

        bool empty() const
        {
            return _table.empty();
        }

        PointerType data()
        {
            return _table.data();
        }

        ConstPointerType data() const
        {
            return _table.data();
        }


        Set& operator=(const Set& rhs);

    private:
        TableType _table;
    };

    template <typename T, typename Allocator>
    Set<T, Allocator>& Set<T, Allocator>::operator=(const Set& rhs)
    {
        if (this != &rhs)
            _table = rhs._table;
        return *this;
    }
}  // namespace Rt2
