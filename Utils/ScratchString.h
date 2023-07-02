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
#include "Utils/String.h"

namespace Rt2
{
    class ScratchString
    {
    public:
        using Buffer   = SimpleArray<char>;
        using SizeType = Buffer::SizeType;
    private:
        Buffer         _buf;
        mutable bool   _dirty{true};
        mutable String _cache;
    public:
        ScratchString()  = default;
        ~ScratchString() = default;

        void reserve(const SizeType& size)
        {
            _buf.reserve(size);
        }

        void push(const char ch)
        {
            _dirty = true;
            _buf.push_back(ch);
        }

        void push(const String& str)
        {
            _dirty = true;
            for (const auto& ch : str)
                _buf.push_back(ch);
        }

        const String& value() const
        {
            if (_dirty)
            {
                _dirty = false;
                _cache = {_buf.data(), _buf.size()};
            }
            return _cache;
        }

        void clear()
        {
            _dirty = true;
            _buf.resizeFast(0);
        }
    };

}  // namespace Rt2
