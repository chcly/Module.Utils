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
#include "Utils/LogFile.h"

namespace Rt2
{
    String* Log::path = nullptr;

    void Log::flush(const OutputStringStream& ss)
    {
        if (path)
        {
            OutputFileStream fs = OutputFileStream();
            fs.open(*path, std::ios_base::app | std::ios_base::ate);
            fs << ss.str();
        }
        else
            Console::print(ss.str());
    }

    void Log::open(const String& log)
    {
        close();
        OutputFileStream fs = OutputFileStream();
        fs.open(log);
        if (!fs.is_open())
        {
            Console::println("Failed to open log ", log);
            return;
        }
        path = new String(log);
    }

    void Log::close()
    {
        delete path;
        path = nullptr;
    }

    void Log::clear()
    {
        if (path)
        {
            OutputFileStream fs = OutputFileStream();
            fs.open(*path, std::ios_base::trunc);
        }
    }

    String Log::current()
    {
        if (path) return *path;
        return {};
    }
}  // namespace Rt2
