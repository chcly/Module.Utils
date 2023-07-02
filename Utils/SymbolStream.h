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
    using SymbolArray = SimpleArray<char>;

    class SymbolStream
    {
    public:
        using Initializer = std::initializer_list<char>;

        static constexpr Initializer Bin = {'0', '1'};
        static constexpr Initializer Hex = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

        static constexpr Initializer HexA = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'};
        static constexpr Initializer HexB = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'};

        static constexpr Initializer Base10 = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    private:
        uint64_t    _base{10};
        uint64_t    _shift{0};
        uint64_t    _cpb{Npos};
        SymbolArray _symbols;
        SymbolArray _scratch;
        bool        _pad{false};
        bool        _ws{false};

    public:
        explicit SymbolStream(const SymbolArray& symbols = {});
        explicit SymbolStream(const Initializer& symbols);
        explicit SymbolStream(const String& symbols);

        void base(OStream& dest, uint64_t v);

        void base(String& dest, uint64_t v);

        void base(OStream& dest, IStream& in);

        void base(String& dest, IStream& in);

        void base(String& dest, const String& in);

        void setBase(int i);

        void setPad(bool v);

        void setWs(bool v);

        void setShift(int offs);

        size_t size() const { return _symbols.size(); }

        static String toString(I8 v, const Initializer& sym, int offs = 0);

        static String toString(I16 v, const Initializer& sym, int offs = 0);

        static String toString(I32 v, const Initializer& sym, int offs = 0);

        static String toString(I64 v, const Initializer& sym, int offs = 0);

        static String toString(U8 v, const Initializer& sym, int offs = 0);

        static String toString(U16 v, const Initializer& sym, int offs = 0);

        static String toString(U32 v, const Initializer& sym, int offs = 0);

        static String toString(U64 v, const Initializer& sym, int offs = 0);

        static String toString(const String &v, const Initializer& sym, int offs = 0);

        static void toString(String& dest, U64 v, const Initializer& sym, int offs = 0);

        static void toString(String& dest,  const String &v, const Initializer& sym, int offs = 0);

    private:
        uint64_t charsPerBase();

        void makeSymbolDefault();
    };

    inline void SymbolStream::setBase(const int i)
    {
        _base = i;
        _cpb  = Npos;
    }

    inline void SymbolStream::setPad(const bool v)
    {
        _pad = v;
    }

    inline void SymbolStream::setWs(const bool v)
    {
        _ws = v;
    }

    inline void SymbolStream::setShift(const int offs)
    {
        _shift = offs;
    }

}  // namespace Rt2
