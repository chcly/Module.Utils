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

#include "Array.h"
#include "Utils/String.h"

namespace Rt2
{
    using SymbolArray = SimpleArray<char>;

    class SymbolStream
    {
    private:
        IStream*    _in{nullptr};
        uint64_t    _base{10};
        uint64_t    _shift{0};
        uint64_t    _cpb{Npos};
        SymbolArray _symbols;
        SymbolArray _scratch;
        bool        _pad{false};
        bool        _ws{false};

    public:
        explicit SymbolStream(const SymbolArray& symbols = {});

        void base(OStream& dest, uint64_t v);

        void base(String& dest, uint64_t v);

        void base(OStream& dest, IStream& in);

        void base(String& dest, IStream& in);

        void base(String& dest, const String& in);

        void setBase(int i);

        void setPad(bool v);

        void setWs(bool v);

        size_t size() const { return _symbols.size(); }

    private:
        uint64_t charsPerBase();

        void makeSymbolDefault();
    };
}  // namespace Rt2
