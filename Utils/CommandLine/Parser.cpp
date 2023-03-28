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
#include "Utils/CommandLine/Parser.h"
#include <iostream>
#include "Utils/Console.h"
#include "Utils/Definitions.h"
#include "Utils/FileSystem.h"
#include "Utils/StackStream.h"
#include "Utils/StreamConverters/Tab.h"

namespace Rt2::CommandLine
{
    using namespace std;

    Parser::~Parser()
    {
        for (const ParseOption* op : _options)
            delete op;
    }

    bool Parser::hasSwitch(const String& sw) const
    {
        return _switches.find(sw) != _switches.end();
    }

    bool Parser::setup(const int argc, char** argv)
    {
        RT_ASSERT(argv)
        RT_ASSERT(argc >= 1)

        if (!_program.empty())  // using as a check for multiple calls
            return false;

        _program = Directory::Path(argv[0]);

        _usedOptions = 0;

        _scanner.load(argv, argc);
        return true;
    }

    int Parser::parseImpl()
    {
        String tmpBuffer;
        Token  token;

        tmpBuffer.reserve(16);
        const int rc = parseOptions(token, tmpBuffer);
        if (rc < 0)
            usage();
        if (_usedOptions != _requiredOptions)
        {
            usage();
            return error("missing required options");
        }
        return rc;
    }

    int Parser::parse(const int      argc,
                      char**         argv,
                      const Switch*  switches,
                      const uint32_t count)
    {
        if (!setup(argc, argv))
            return -1;

        if (!initializeSwitches(switches, count))
            return -1;

        return parseImpl();
    }

    int Parser::parse(const int argc, char** argv)
    {
        if (!setup(argc, argv))
            return -1;
        return parseImpl();
    }

    int Parser::parseOptions(Token& token, String& tmpBuffer)
    {
        while (token.getType() != TOK_EOS)
        {
            _scanner.scan(token);
            const int type = token.getType();
            if (token.getType() == TOK_ERROR)
                return error("error token, ", token.getValue());

            if (type == TOK_SHORT || type == TOK_LONG)
            {
                // grab the next token
                _scanner.scan(token);
                if (token.getType() != TOK_VALUE)
                {
                    return error(
                        "expected a switch value "
                        "to follow the '-' character");
                }

                if (token.getValue() == "help" || token.getValue() == "h")
                {
                    _usedOptions = _requiredOptions;
                    usage();
                    return -1;
                }

                auto it = _switches.find(token.getValue());
                if (it == _switches.end())
                    return error("unknown option ", token.getValue());

                tmpBuffer.assign(token.getValue());

                ParseOption* opt = it->second;
                opt->makePresent();
                if (!opt->isOptional())
                    _usedOptions++;

                if (const size_t nArgs = opt->getArgCount();
                    nArgs > 0)
                {
                    size_t i;
                    for (i = 0; i < nArgs; ++i)
                    {
                        _scanner.scan(token);
                        if (token.getType() != TOK_VALUE)
                        {
                            return error("invalid argument for option: ",
                                         tmpBuffer);
                        }

                        if (token.getValue().empty())
                        {
                            return error("missing argument for option: ",
                                         tmpBuffer);
                        }

                        opt->setValue(i, token.getValue());
                    }

                    if (i != nArgs)
                    {
                        return error(
                            "not all arguments converted "
                            "when parsing switch: ",
                            tmpBuffer);
                    }
                }
            }
            else if (type == TOK_VALUE)
                _argumentList.push_back(token.getValue());
            else
            {
                if (type != TOK_EOS)
                    return error("unknown option: ",
                                 token.getValue());
            }
        }
        return 0;
    }

    String Parser::programPath() const
    {
        return _program.fullPlatform();
    }

    String Parser::programName() const
    {
        return _program.base();
    }

    String Parser::programDirectory() const
    {
        return _program.fullDirectory();
    }

    String Parser::currentDirectory()
    {
        return Directory::Path::current().full();
    }

    bool Parser::isPresent(const uint32_t& enumId) const
    {
        if (enumId < _options.size())
            return _options[enumId]->isPresent();
        return false;
    }

    int Parser::flag(const uint32_t& enumId,
                     const int       present,
                     const int       notPresent) const
    {
        return isPresent(enumId) ? present : notPresent;
    }

    void Parser::setIfPresent(int& dest, const uint32_t& enumId, const int present) const
    {
        if (isPresent(enumId))
            dest |= present;
    }

    ParseOption* Parser::option(const uint32_t& enumId) const
    {
        if (enumId < _options.size())
            return _options[enumId];
        return nullptr;
    }

    int32_t Parser::int32(const uint32_t& enumId,
                          const size_t    idx,
                          const int32_t   defaultValue,
                          const int32_t   base) const
    {
        if (enumId < _options.size())
        {
            if (_options[enumId] != nullptr)
                return _options[enumId]->getInt(idx, defaultValue, base);
        }
        return defaultValue;
    }

    int64_t Parser::int64(const uint32_t& enumId,
                          const size_t    idx,
                          const int64_t   defaultValue,
                          const int32_t   base) const
    {
        if (enumId < _options.size())
        {
            if (_options[enumId] != nullptr)
                return _options[enumId]->getInt64(idx, defaultValue, base);
        }
        return defaultValue;
    }

    const String& Parser::string(const uint32_t& enumId,
                                 const size_t    idx,
                                 const String&   defaultValue) const
    {
        if (enumId < _options.size())
        {
            if (_options[enumId] != nullptr)
            {
                const String& val = _options[enumId]->getValue(idx);
                if (val.empty())
                    return defaultValue;
                return val;
            }
        }
        return defaultValue;
    }

    void Parser::usage() const
    {
        String usageString;
        usage(usageString);
        Console::writeLine(usageString);
    }

    void Parser::usage(String& dest) const
    {
        const int mw = max<int>(_maxLen, 6);

        OutputStringStream stream;
        TStreamStack<2>    out;

        auto lock = out.push(&stream);

        out.endl(1);
        out.println("Usage: ", programName(), " [<options> <arg[0] .. arg[n]>]");
        out.endl();
        out.inc();
        out.println("Where <arg[0] .. arg[n]> is a white space separated list");
        out.println(" of input arguments, and one or more of the following:");
        out.endl();
        out.println("<options>");
        out.endl();
        out.inc();

        out.println("-h, --help", Tab(mw - 4), " - Display this message.");

        for (const auto opt : _options)
        {
            const auto& [id,
                         shortSwitch,
                         longSwitch,
                         description,
                         optional,
                         argCount] = opt->getSwitch();

            if (shortSwitch != 0)
            {
                out.print('-', (char)shortSwitch);
                if (longSwitch != nullptr)
                    out.print(',', ' ');
                else
                    out.print(Tab(2));
            }
            else
                out.print(Tab(4));

            if (longSwitch != nullptr)
            {
                out.print('-', '-', longSwitch);
                out.print(Tab(mw - (int)Char::length(longSwitch)));
            }
            else
                out.print(Tab(mw + 2));

            out.print(' ', (optional ? '-' : '+'), ' ');

            if (description != nullptr)
            {
                String desc = description;
                if (!Su::endsWith(desc, '.'))
                    desc.push_back('.');

                StringArray arr;
                StringUtils::splitRejectEmpty(arr, desc, '\n');
                for (size_t i = 0; i < arr.size(); ++i)
                {
                    if (i == 0)
                        out.println(Su::toUpperFirst(arr[i]));
                    else
                        out.println(Tab(4 + mw + 2 + 4), arr[i]);
                }
                if (arr.size() > 1)  // break the line to improve readability
                    out.endl();
            }
            else
                out.endl();
        }
        out.endl();
        out.println("'+' Is a required parameter.");
        out.flush();
        dest = stream.str();
    }

    bool Parser::initializeOption(ParseOption* opt, const Switch& sw)
    {
        if (sw.shortSwitch == 0 && sw.longSwitch == nullptr)
        {
            Console::writeError(
                "A switch must have at least a long or short name");
            return false;
        }

        if (sw.shortSwitch != 0)
        {
            if (hasSwitch(String(sw.shortSwitch, 1)))
            {
                Console::writeError("Duplicate switch: ", (char)sw.shortSwitch);
                return false;
            }
        }

        if (sw.longSwitch != nullptr)
        {
            const String lsw = String(sw.longSwitch);

            _maxLen = max<int>(_maxLen, (int)lsw.size());

            if (hasSwitch(lsw))
            {
                Console::writeError("Duplicate switch: ", (char)sw.shortSwitch);
                return false;
            }
        }

        if (sw.shortSwitch != 0)
            _switches.insert(make_pair(String(1, sw.shortSwitch), opt));

        if (sw.longSwitch != nullptr)
            _switches.insert(make_pair(sw.longSwitch, opt));

        return true;
    }

    bool Parser::initializeSwitches(const Switch* switches, uint32_t count)
    {
        if (switches == nullptr || count == 0 || count == (uint32_t)-1)
            return true;

        count = min<uint32_t>(count, 0x100);

        _options.resize(count);
        bool result = true;

        for (uint32_t i = 0; i < count && result; ++i)
        {
            if (switches[i].id != i)
            {
                Console::writeError("misaligned switch id");
                result = false;
            }
            else
            {
                if (!switches[i].optional)
                    _requiredOptions++;

                _options[i] = new ParseOption(switches[i]);

                result = initializeOption(_options[i], switches[i]);
            }
        }
        return result;
    }

    int Parser::writeError(const OutputStringStream& stream) const
    {
        Console::println(programPath());
        Console::nl();
        Console::setForeground(CS_RED);
        Console::println(Tab(4), stream.str());
        return -1;
    }

}  // namespace Rt2::CommandLine
