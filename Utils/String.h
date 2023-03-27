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

#include <deque>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Rt2
{
    using String      = std::string;
    using StringDeque = std::deque<std::string>;
    using StringArray = std::vector<std::string>;
    using StringMap   = std::unordered_map<std::string, std::string>;

    using IStream = std::istream;
    using OStream = std::ostream;

    using StringStream       = std::stringstream;
    using InputStringStream  = std::istringstream;
    using OutputStringStream = std::ostringstream;
    using InputFileStream    = std::ifstream;
    using OutputFileStream   = std::ofstream;
    using FilterFunction     = std::function<bool(const char& ch)>;

    class StringUtils
    {
    public:
        static void split(
            StringArray&  destination,
            const String& input,
            char          separator);

        static void splitRejectEmpty(
            StringArray&  destination,
            const String& input,
            char          separator);

        static void split(
            StringArray&  destination,
            const String& input,
            const String& separator);

        static void split(
            StringDeque&  destination,
            const String& input,
            const String& separator);

        static void splitLine(
            StringArray&  dest,
            const String& input,
            char          swap = '$');

        static void scanSplitLine(
            StringArray&  dest,
            const String& input);

        static void combine(
            String&            dest,
            const StringDeque& input,
            char               in  = 0,
            char               out = 0);

        static void trimWs(
            String&       di,
            const String& in);

        static bool startsWith(
            const String& test,
            const char&   chk);

        static bool startsWith(
            const String& test,
            const String& chk);

        static bool endsWith(
            const String& test,
            const char&   chk);

        static bool endsWith(
            const String& test,
            const String& chk);

        static void trimL(
            String&       destination,
            const String& input,
            char          character);

        static void trimR(
            String&       destination,
            const String& input,
            char          character);

        static void trimRelPath(
            String&       destination,
            const String& input);

        static void trim(
            String&       destination,
            const String& input,
            char          character);

        /// Filters characters out of the input string into the destination string.
        /// Filtering occurs when the pass function returns a true value.
        /// Returns true if the string was modified.
        static bool filter(
            String&               destination,
            const String&         input,
            const FilterFunction& pass,
            size_t                max = 0x100);

        static bool filterRange(
            String&       destination,
            const String& input,
            int8_t        start,
            int8_t        end,
            size_t        max = 0x100);

        static bool filterAZaz(
            String&       destination,
            const String& input,
            size_t        max = 0x100);

        static bool filterAZaz09(
            String&       destination,
            const String& input,
            size_t        max = 0x100);

        static bool filterAscii(
            String&       destination,
            const String& input,
            size_t        max = 0x100);

        static bool filterInt(
            String&       destination,
            const String& input,
            size_t        max = 0x100);

        static bool filterReal(
            String&       destination,
            const String& input,
            size_t        max = 0x100);

        static void replaceAll(
            String&       dest,
            const String& input,
            const String& a,
            const String& b);

        static void reverse(
            String&       dest,
            const String& input);

        static void toLower(
            String&       dest,
            const String& in);

        static void toUpper(
            String&       dest,
            const String& in);

        static String toLowerFirst(
            const String& in);

        static String toUpperFirst(
            const String& in);

        static void copy(
            OStream& out,
            IStream& in,
            bool     binary  = true,
            bool     newLine = true);

        static void copy(
            String&  out,
            IStream& in,
            bool     newLine = true);

        static void scramble(
            String& destination,
            size_t  value,
            bool    randomize = true);

        static String scramble(
            const String& value,
            bool          randomize = true);

        static void generate(
            String& destination,
            int&    counter,
            void*   seed);

        template <typename... Args>
        static String join(Args&&... args)
        {
            OutputStringStream oss;
            ((oss << std::forward<Args>(args)), ...);
            return oss.str();
        }

        template <typename... Args>
        static OStream& merge(OStream& out, Args&&... args)
        {
            ((out << std::forward<Args>(args)), ...);
            return out;
        }

        template <typename... Args>
        static String csv(const char c, Args&&... args)
        {
            OutputStringStream out;
            ((out << c << std::forward<Args>(args)), ...);
            return out.str();
        }
    };

    using Su = StringUtils;

}  // namespace Rt2
