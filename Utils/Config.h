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
#include "Utils/String.h"

namespace Rt2
{

    /**
     * \brief Defines a basic key value parser
     *
     * Defined as:
     * \code{.txt}
     * key = "value";
     * \endcode
     *
     * A valid key should match the following expression:\n
     * \code{.txt}
     * [a-zA-Z][a-zA-Z0-9]+[_-.]*
     * \endcode
     * A valid value should match the following expression:\n
     * \code{.txt}
     * [a-zA-Z0-9]*[ \t_-.:#,_-./@()]*
     * \endcode
     */
    class Config
    {
    private:
        StringMap _attributes;

        static void scanToQuote(IStream& input);

        static void scanValue(String& dest, IStream& input);
        static void scanKey(String& dest, IStream& input);
        static void scanComment(IStream& input);
        static char next(IStream& input);

    public:
        Config();

        ~Config() = default;

        void load(IStream& input);

        bool contains(const String& key);

        const String& string(const String& key, const String& def = "");

        bool boolean(const String& key, bool def = false);

        int integer(const String& key, int def = 0);

        void csv(const String& key, StringArray &values);

        const StringMap& attributes() const;
    };

}  // namespace Rt2
