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
#include "Utils/Config.h"
#include "Exception.h"
#include "Utils/Char.h"

namespace Rt2
{
    Config::Config() = default;

    void Config::load(const String& fn)
    {
        if (InputFileStream ifs(fn);
            ifs.is_open())
            load(ifs);
        else
            throw Exception("failed to load: ", fn);
    }

    void Config::load(IStream& input)
    {
        String key, value;

        while (!input.eof())
        {
            const char ch = next(input);
            if (ch <= 0)
                break;

            switch (ch)
            {
            case '\r':
            case '\n':
            case ' ':
            case '\t':
                break;
            case '#':
                scanComment(input);
                break;
            case LowerCaseAz:
            case UpperCaseAz:
                input.putback(ch);
                value.clear();
                key.clear();
                scanKey(key, input);
                break;
            case '=':
                if (key.empty())
                    throw Exception("assignment operator without an identifier");

                value.clear();
                scanValue(value, input);

                if (!value.empty())
                {
                    _attributes[key] = value;

                    value.clear();
                    key.clear();
                }
                break;
            default:
                throw Exception("unknown character parsed '", (int)ch, '\'');
            }
        }
    }

    bool Config::contains(const String& key)
    {
        const StringMap::const_iterator it = _attributes.find(key);
        return it != _attributes.end();
    }

    void Config::scanToQuote(IStream& input)
    {
        while (!input.eof())
        {
            const char ch = next(input);
            switch (ch)
            {
            // allow only white space and a quote
            // after an = character
            case '\"':
                return;
            case ' ':
            case '\t':
                break;
            default:
                throw Exception("unknown character parsed '", (int)ch, '\'');
            }
        }
        throw Exception("end of file scan");
    }

    void Config::scanValue(String& dest, IStream& input)
    {
        if (input.peek() != '\"')
            scanToQuote(input);
        else
        {
            // it does start with a quote
            // so skip it.
            next(input);
        }

        while (!input.eof())
        {
            const char ch = next(input);

            switch (ch)
            {
            case ':':
            case '#':
            case ',':
            case '_':
            case '-':
            case '.':
            case '/':
            case '@':
            case '(':
            case ')':
            case ' ':
            case '\t':
            case Digits09:
            case LowerCaseAz:
            case UpperCaseAz:
                dest.push_back(ch);
                break;
            case '\"':
                if (input.peek() != ';')
                    throw Exception("expected a terminating semicolon");
                next(input);
                return;
            default:
                throw Exception("unknown character parsed '", ch, '\'');
            }
        }

        throw Exception("end of file scan");
    }

    void Config::scanKey(String& dest, IStream& input)
    {
        bool lock = false;
        while (!input.eof())
        {
            const char ch = next(input);

            switch (ch)
            {
            // A valid id is composed of the expression
            // [a-zA-Z][a-zA-Z0-9]+[_-.]*
            case '_':
            case '-':
            case '.':
            case Digits09:
            case LowerCaseAz:
            case UpperCaseAz:
                if (!lock)
                    dest.push_back(ch);
                else
                    throw Exception("unknown character parsed '", ch, '\'');
                break;
            case '\t':
            case ' ':
                // on the first occurrence of white space, lock the scan
                // and only accept [ \t=], where = is the exit condition.
                lock = true;
                break;
            case '=':
                input.putback(ch);
                return;
            default:
                throw Exception("unknown character parsed '", (int)ch, '\'');
            }
        }

        // Do not allow the assignment to be partially defined.
        throw Exception("end of file scan");
    }

    void Config::scanComment(IStream& input)
    {
        while (!input.eof())
        {
            const char ch = next(input);

            switch (ch)
            {
            case '\r':
            case '\n':
                if (ch == '\r' && input.peek() == '\n')
                    next(input);
                return;
            default:
                break;
            }
        }

        // Allow EOF scan in comments
    }

    char Config::next(IStream& input)
    {
        if (input.eof())
            return 0;

        char ch;
        input.read(&ch, 1);
        return ch;
    }

    const String& Config::string(const String& key, const String& def)
    {
        if (const StringMap::const_iterator it = _attributes.find(key);
            it != _attributes.end())
            return it->second;
        return def;
    }

    bool Config::boolean(const String& key, const bool def)
    {
        const String& v = string(key);
        if (v != "true")
            return def;
        return true;
    }

    int Config::integer(const String& key, const int def)
    {
        return Char::toInt32(string(key), def);
    }

    void Config::csv(const String& key, StringArray& values)
    {
        Su::splitRejectEmpty(values, string(key), ',');
    }

    const StringMap& Config::attributes() const
    {
        return _attributes;
    }

}  // namespace Rt2
