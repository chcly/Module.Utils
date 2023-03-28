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
#include "Utils/CommandLine/Scanner.h"
#include "Utils/Char.h"
#include "Utils/Exception.h"
#include "Utils/StreamConverters/Hex.h"

namespace Rt2::CommandLine
{

    void Scanner::load(char** argv, const int argc)
    {
        String buf;
        Su::cmd(buf, argv, argc);
        _stream = InputStringStream(buf);
    }

    void Scanner::scanString(Token& tok)
    {
        int ch = _stream.get();
        if (!isQuote(ch))
            throw Exception("expected the quote character '\"'");

        ch = _stream.get();
        while (!isQuote(ch))
        {
            tok.append((char)ch);
            ch = _stream.get();
            if (ch <= 0)
            {
                throw Exception(
                    "unexpected end of file "
                    "while searching for a closing (",
                    '"',
                    ") character");
            }
        }
        tok.setType(TOK_VALUE);
    }

    inline bool isValid(const int ch)
    {
        return !isWhiteSpace(ch) && ch > 0;
    }

    void Scanner::scanOption(Token& tok)
    {
        int ch = _stream.get();
        while (isValid(ch))
        {
            tok.append((char)ch);
            ch = _stream.get();
        }


        tok.setType(TOK_VALUE);
    }

    void Scanner::scan(Token& tok)
    {
        tok.clear();
        while (!_stream.eof())
        {
            switch (const int ch = _stream.get())
            {
            case '-':
            {
                if (_stream.peek() == '-')
                {
                    _stream.seekg(1, std::ios_base::cur);
                    tok.setType(TOK_LONG);
                }
                else
                    tok.setType(TOK_SHORT);
                return;
            }
            case '"':
            case '\'':
                // read any '[.]*' or "[.]*"
                _stream.putback((char)ch);
                scanString(tok);
                return;
            case '.':
            case '/':
            case Digits09:
            case UpperCaseAz:
            case LowerCaseAz:
                // read any till the eof or first ' '
                _stream.putback((char)ch);
                scanOption(tok);
                return;
            case ' ':
            case '\t':
                break;
            case 0:
            case -1:
                tok.setType(TOK_EOS);
                return;
            default:
                tok.setType(TOK_ERROR);
                tok.append(Su::join("unknown token, ", Hex(ch)));
                return;
            }
        }
    }
}  // namespace Rt2::CommandLine
