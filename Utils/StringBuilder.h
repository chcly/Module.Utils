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

#include "Definitions.h"
#include "Utils/String.h"

namespace Rt2
{

    enum StringBuilderStrategy
    {
        ALLOC_N_BYTE_BLOCK,
        ALLOC_MUL2,
    };

    class StringBuilder
    {
    private:
        I8*    _buffer{nullptr};
        size_t _size{0};
        size_t _capacity{0};
        size_t _nByteBlock{256};
        U8     _strategy{ALLOC_N_BYTE_BLOCK};
        String _scratch{};

        void reserve(size_t len);

        size_t getNextCapacity(size_t len) const;

        size_t writeToBuffer(const void* source, size_t len);

    public:
        StringBuilder()
        {
            setStrategy(ALLOC_N_BYTE_BLOCK, _nByteBlock);
        }

        explicit StringBuilder(const size_t nBytes)
        {
            setStrategy(ALLOC_N_BYTE_BLOCK, nBytes);
        }

        StringBuilder(const U8 strategy, const size_t nBytes)
        {
            setStrategy(strategy, nBytes);
        }

        ~StringBuilder()
        {
            clear();
        }

        void setStrategy(U8 strategy, size_t nBytes);

        size_t size() const;

        size_t capacity() const;

        String toString() const;

        void toString(String& dest) const;

        void clear();

        void write(const StringBuilder& oth);

        void write(const String& str);

        void write(char ch);

        void write(const char* ptr);

        void write(const U8* ptr, size_t len);

        void write(I16 val);

        void write(I32 val);

        void write(I64 val);

        void write(U16 val);

        void write(U32 val);

        void write(U64 val);

        void write(double val);

        void write(float val);
    };

    inline size_t StringBuilder::size() const
    {
        return _size;
    }

    inline size_t StringBuilder::capacity() const
    {
        return _capacity;
    }

}  // namespace Rt2
