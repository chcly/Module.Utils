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
#include "Utils/SymbolStream.h"
#include <cmath>
#include "TextStreamWriter.h"
#include "Utils/String.h"

namespace Rt2
{

    SymbolStream::SymbolStream(const SymbolArray& symbols) :
        _symbols(symbols)
    {
        if (_symbols.empty())
            makeSymbolDefault();
    }

    SymbolStream::SymbolStream(const Initializer& symbols) :
        _base(symbols.size()),
        _symbols(symbols)
    {
        if (_symbols.empty())
        {
            makeSymbolDefault();
            _base = _symbols.size();
        }
    }

    SymbolStream::SymbolStream(const String& symbols) :
        _base(symbols.size())
    {
        for (const char ch : symbols)
            _symbols.push_back(ch);
    }

    void SymbolStream::base(OStream& dest, uint64_t v)
    {
        _scratch.resizeFast(0);
        uint64_t q, r;
        uint32_t idx;

        if (_base < 2)
            return;

        if (v == 0)
        {
            r = 0;
            if (_shift > 0)
            {
                r += _shift;
                r %= _base;
            }
            idx = r % _symbols.size();
            _scratch.push_back(_symbols.at(idx));
        }
        else
        {
            while (v > 0)
            {
                q = v / _base;
                r = v % _base;

                if (r < (uint64_t)_symbols.size())
                {
                    if (_shift > 0)
                    {
                        r += _shift;
                        r %= _base;
                    }
                    idx = r % _symbols.size();
                    _scratch.push_back(_symbols.at(idx));
                }
                v = q;
            }
        }

        if (_pad)
        {
            while (_scratch.size() < charsPerBase())
            {
                r = 0;
                if (_shift > 0)
                {
                    r += _shift;
                    r %= _base;
                }
                idx = r % _symbols.size();
                _scratch.push_back(_symbols.at(idx));
            }
        }
        if (_ws)
        {
            r = Max<int>(0, (int)charsPerBase() - (int)_scratch.size());
            for (q = 0; q < r; ++q)
                Ts::print(dest, ' ');
            Ts::print(dest, ' ');
        }

        for (int i = _scratch.sizeI() - 1; i >= 0; --i)
            Ts::print(dest, _scratch[i]);
    }

    void SymbolStream::base(String& dest, uint64_t v)
    {
        OutputStringStream oss;
        base(oss, v);
        dest = oss.str();
    }

    void SymbolStream::base(OStream& dest, IStream& in)
    {
        char ch;
        while (!in.read(&ch, 1).eof())
        {
            if (ch > 0)
                base(dest, (uint64_t)ch);
        }
    }

    void SymbolStream::base(String& dest, IStream& in)
    {
        OutputStringStream oss;
        base(oss, in);
        dest = oss.str();
    }

    void SymbolStream::base(String& dest, const String& in)
    {
        InputStringStream  is(in);
        OutputStringStream oss;
        base(oss, is);
        dest = oss.str();
    }

    String SymbolStream::toString(const I8 v, const Initializer& sym, const int offs)
    {
        return toString((uint64_t)v, sym, offs);
    }

    String SymbolStream::toString(const I16 v, const Initializer& sym, const int offs)
    {
        return toString((uint64_t)v, sym, offs);
    }

    String SymbolStream::toString(const I32 v, const Initializer& sym, const int offs)
    {
        return toString((uint64_t)v, sym, offs);
    }

    String SymbolStream::toString(const I64 v, const Initializer& sym, const int offs)
    {
        return toString((uint64_t)v, sym, offs);
    }

    String SymbolStream::toString(const U8 v, const Initializer& sym, const int offs)
    {
        return toString((uint64_t)v, sym, offs);
    }

    String SymbolStream::toString(const U16 v, const Initializer& sym, const int offs)
    {
        return toString((uint64_t)v, sym, offs);
    }

    String SymbolStream::toString(const U32 v, const Initializer& sym, const int offs)
    {
        return toString((uint64_t)v, sym, offs);
    }

    String SymbolStream::toString(const U64 v, const Initializer& sym, const int offs)
    {
        String copy;
        toString(copy, v, sym, offs);
        return copy;
    }

    String SymbolStream::toString(const String& v, const Initializer& sym, const int offs)
    {
        String copy;
        toString(copy, v, sym, offs);
        return copy;
    }

    void SymbolStream::toString(String& dest, const U64 v, const Initializer& sym, const int offs)
    {
        SymbolStream ss(sym);
        ss.setShift(offs);
        ss.base(dest, v);
    }

    void SymbolStream::toString(String& dest, const String& v, const Initializer& sym, const int offs)
    {
        SymbolStream ss(sym);
        ss.setShift(offs);
        ss.base(dest, v);
    }

    size_t SymbolStream::charsPerBase()
    {
        if (_cpb == Npos)
            _cpb = Clamp(int(ceil(log(255.0) / log((double)_base))), 2, 8);
        return _cpb;
    }

    void SymbolStream::makeSymbolDefault()
    {
        _symbols.reserve(96);  // +1
        int i;
        for (i = 0; i < 10; ++i)
            _symbols.push_back((char)int('0' + i));
        for (i = 0; i < 26; ++i)
            _symbols.push_back((char)int('A' + i));
        for (i = 0; i < 26; ++i)
            _symbols.push_back((char)int('a' + i));
        for (i = 32; i <= 47; ++i)
            _symbols.push_back((char)i);
        for (i = 58; i <= 64; ++i)
            _symbols.push_back((char)i);
        for (i = 91; i <= 96; ++i)
            _symbols.push_back((char)i);
        for (i = 123; i <= 126; ++i)
            _symbols.push_back((char)i);
    }
}  // namespace Rt2
