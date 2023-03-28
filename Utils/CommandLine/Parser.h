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
#include <unordered_map>
#include "Utils/CommandLine/Option.h"
#include "Utils/CommandLine/Scanner.h"
#include "Utils/Directory/Path.h"
#include "Utils/FileSystem.h"
#include "Utils/Path.h"

namespace Rt2::CommandLine
{
    class Parser
    {
    public:
        typedef std::unordered_map<String, ParseOption*> Switches;
        typedef std::vector<ParseOption*>                Options;
        typedef std::vector<String>                      StringArray;

    private:
        int             _maxLen{4};
        int             _requiredOptions{0};
        int             _usedOptions{0};
        Scanner         _scanner;
        Switches        _switches;
        StringArray     _argumentList;
        Options         _options;
        Directory::Path _program;

        bool hasSwitch(const String& sw) const;

        bool initializeOption(ParseOption* opt, const Switch& sw);

        bool initializeSwitches(const Switch* switches, uint32_t count);

        int writeError(const OutputStringStream& stream) const;

        int parseOptions(Token& token, String& tmpBuffer);

        bool setup(int argc, char** argv);

        int parseImpl();

        template <typename... Args>
        int error(Args&&... args)
        {
            OutputStringStream out;
            ((out << std::forward<Args>(args)), ...);
            out << std::endl;
            return writeError(out);
        }

    public:
        Parser() = default;
        ~Parser();

        int parse(int argc, char** argv, const Switch* switches, uint32_t count);

        int parse(int argc, char** argv);

        StringArray& arguments();

        String programPath() const;

        String programName() const;

        String programDirectory() const;

        static String currentDirectory();

        bool isPresent(const uint32_t& enumId) const;

        int flag(const uint32_t& enumId, int present, int notPresent) const;

        void setIfPresent(int& dest, const uint32_t& enumId, int present) const;

        ParseOption* option(const uint32_t& enumId) const;

        int32_t int32(const uint32_t& enumId,
                      size_t          idx          = 0,
                      int32_t         defaultValue = -1,
                      int32_t         base         = 10) const;

        int64_t int64(const uint32_t& enumId,
                      size_t          idx          = 0,
                      int64_t         defaultValue = -1,
                      int32_t         base         = 10) const;

        const String& string(const uint32_t& enumId,
                             size_t          idx          = 0,
                             const String&   defaultValue = "") const;

        void usage() const;

        void usage(String& dest) const;
    };

    inline StringArray& Parser::arguments()
    {
        return _argumentList;
    }
}  // namespace Rt2::CommandLine
