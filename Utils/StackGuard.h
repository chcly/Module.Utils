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

#include "Utils/Definitions.h"
#include "Utils/Exception.h"

namespace Rt2
{
    // This Should be reset before any recursive call.
    // On each level of recursion, use the test method to check the depth.
    // If test returns true the limit has been reached, and the recursive call
    // should start exiting.
    template <I32 Max>
    class FixedStackGuard
    {
    private:
        mutable I32 _depth{0};

        static_assert(Max > 0);
        static_assert(Max < 0xFFFF);

        FixedStackGuard(const FixedStackGuard&) = default;

    public:
        FixedStackGuard() = default;

        // If test returns true the limit has been reached, and the recursive call
        // should start exiting.
        bool test() const
        {
            const bool result = _depth >= Max;
            if (!result)
                ++_depth;
            return result;
        }

        void release() const
        {
            if ((int)_depth <= 0)
                _depth = 0;
            else
                --_depth;
        }

        void reset() 
        {
            // keep immutable..
            _depth = 0;
        }

        I32 depth() const
        {
            return _depth;
        }

        bool isValid() const
        {
            // has at least one more.
            return _depth + 1 <= Max;
        }
    };

}  // namespace Rt2
