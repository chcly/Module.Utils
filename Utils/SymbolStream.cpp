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
#include <cmath>
#include "Utils/SymbolStream.h"
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

    void SymbolStream::base(OStream& dest, uint64_t v)
    {
        _scratch.resizeFast(0);
        uint64_t q, r;

        if (v == 0)
        {
            r = 0;
            if (_shift > 0)
            {
                r += _shift;
                r %= _base;
            }
            _scratch.push_back(_symbols.at(r));
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
                    _scratch.push_back(_symbols.at(r));
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
                _scratch.push_back(_symbols.at(r));
            }
        }

        if (_ws)
        {
            r = (int)Max<size_t>(0, charsPerBase() - (size_t)_scratch.size());
            for (q = 0; q < r; ++q)
                Ts::print(dest, ' ');
            Ts::print(dest, ' ');
        }

        for (int i = _scratch.sizeI() - 1; i >= 0; --i)
            Ts::print(dest, (char)_scratch[i]);
    }

    void SymbolStream::base(String& dest, uint64_t v)
    {
        OutputStringStream oss;
        base(oss, v);
        dest = oss.str();
    }

    void SymbolStream::base(OStream& dest, IStream& in)
    {
        while (!in.eof())
        {
            char ch;
            in.read(&ch, 1);
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

    void SymbolStream::setBase(const int i)
    {
        _base = i;
        _cpb  = Npos;
    }

    void SymbolStream::setPad(const bool v)
    {
        _pad = v;
    }

    void SymbolStream::setWs(const bool v)
    {
        _ws = v;
    }

    size_t SymbolStream::charsPerBase()
    {
        if (_cpb == Npos)
        {
            int ln = int(ceil(log(255.0) / log((double)_base)));
            if (ln < 2)
                ln = 2;
            if (ln > 8)
                ln = 8;
            _cpb = ln;
        }
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
