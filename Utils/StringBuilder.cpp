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
#include "Utils/StringBuilder.h"
#include "Char.h"

namespace Rt2
{
    String StringBuilder::toString() const
    {
        // discouraged,
        // because of copy on return.
        return {(char*)_buffer, size()};
    }

    void StringBuilder::toString(String& dest) const
    {
        dest.clear();
        dest.append({(char*)_buffer, size()});
    }

    void StringBuilder::clear()
    {
        delete[] _buffer;
        _buffer = nullptr;

        _size     = 0;
        _capacity = 0;
    }

    void StringBuilder::reserve(const size_t len)
    {
        const size_t newCap = getNextCapacity(len);

        RT_ASSERT(newCap > _size)
        if (I8* newMem = new I8[newCap + 1]; 
            newMem != nullptr)
        {
            if (_size > 0)
                memcpy(newMem, _buffer, _size);

            delete[] _buffer;
            _buffer   = newMem;
            _capacity = newCap;
        }
    }

    size_t StringBuilder::getNextCapacity(size_t len) const
    {
        size_t next;
        if (_strategy == ALLOC_MUL2)
            next = _capacity * 2;
        else
            next = _capacity + _nByteBlock;
        next += len;
        return next;
    }

    size_t StringBuilder::writeToBuffer(const void* source, const size_t len)
    {
        const size_t nextPosition = _size + len;
        if (nextPosition > _capacity)
        {
            reserve(nextPosition);
            RT_ASSERT(_capacity > nextPosition)
        }

        if (_buffer != nullptr)
        {
            memcpy(&_buffer[_size], source, len);
            _size = nextPosition;
            return len;
        }
        return Npos;
    }

    void StringBuilder::setStrategy(const U8 strategy, const size_t nBytes)
    {
        _strategy   = Clamp<U8>(strategy, 0, 1);
        _nByteBlock = Clamp<size_t>(nBytes, 8, Npos16);
        reserve(_nByteBlock);
    }


    void StringBuilder::write(const StringBuilder& oth)
    {
        if (oth.size() > 0)
            writeToBuffer(oth._buffer, oth.size());
    }

    void StringBuilder::write(const String& str)
    {
        if (!str.empty())
            writeToBuffer(str.c_str(), str.size());
    }

    void StringBuilder::write(const char ch)
    {
        writeToBuffer(&ch, 1);
    }

    void StringBuilder::write(const char* ptr)
    {
        if (ptr && *ptr)
            writeToBuffer(ptr, Char::length(ptr));
    }

    void StringBuilder::write(const U8* ptr, const size_t len)
    {
        if (ptr && *ptr)
            writeToBuffer(ptr, len);
    }

    void StringBuilder::write(const I16 val)
    {
        _scratch.resize(0);
        Char::toString(_scratch, val);
        writeToBuffer(_scratch.c_str(), _scratch.size());
    }

    void StringBuilder::write(const I32 val)
    {
        _scratch.resize(0);
        Char::toString(_scratch, val);
        writeToBuffer(_scratch.c_str(), _scratch.size());
    }

    void StringBuilder::write(const I64 val)
    {
        _scratch.resize(0);
        Char::toString(_scratch, val);
        writeToBuffer(_scratch.c_str(), _scratch.size());
    }

    void StringBuilder::write(const U16 val)
    {
        _scratch.resize(0);
        Char::toString(_scratch, val);
        writeToBuffer(_scratch.c_str(), _scratch.size());
    }

    void StringBuilder::write(const U32 val)
    {
        _scratch.resize(0);
        Char::toString(_scratch, val);
        writeToBuffer(_scratch.c_str(), _scratch.size());
    }

    void StringBuilder::write(const U64 val)
    {
        _scratch.resize(0);
        Char::toString(_scratch, val);
        writeToBuffer(_scratch.c_str(), _scratch.size());
    }

    void StringBuilder::write(const double val)
    {
        _scratch.resize(0);
        Char::toString(_scratch, val);
        writeToBuffer(_scratch.c_str(), _scratch.size());
    }

    void StringBuilder::write(const float val)
    {
        _scratch.resize(0);
        Char::toString(_scratch, val);
        writeToBuffer(_scratch.c_str(), _scratch.size());
    }

}  // namespace Rt2