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
#ifdef _WIN32
    #include "Utils/Win32/Error.h"
    #include <Windows.h>
    #include "Utils/Console.h"
    #include "Utils/StreamMethods.h"

namespace Rt2::Win32
{
    void LogError(const char* message, U32 res)
    {
        const U32 err = GetLastError();
        LPSTR     buf = nullptr;
        FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
            nullptr,
            err,
            0,
            (LPSTR)&buf,
            0,
            nullptr);

        if (buf)
        {
            Console::error("code: ", Hex(err), ", ", buf, " ", message);
            LocalFree(buf);
        }
        else
            Console::error(message, ": (", res, ") ", err);
    }
}  // namespace Rt2::Win32

#endif
