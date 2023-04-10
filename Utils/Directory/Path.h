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

#include "Utils/Array.h"
#include "Utils/FileSystem.h"
#include "Utils/String.h"

namespace Rt2::Directory
{
    class Path;
    struct PathPrivate;

    using PathArray = Array<Path>;

    class Path
    {
    private:
        PathPrivate* _private{nullptr};

    public:
        Path();

        Path(const Path& oth);

        explicit Path(const String& fileName);

        ~Path();

        Path& operator=(const Path& oth);

        Path& operator=(const String& oth);

        bool empty() const;

        const String& root() const;

        const String& directory() const;

        String lastDirectory() const;

        const String& stem() const;

        const String& base() const;

        const String& extension() const;

        String firstExtension() const;

        String lastExtension() const;

        const String& full() const;

        const String& fullDirectory() const;

        Path directoryAsPath() const;

        bool hasDirectory() const;

        bool isRooted() const;

        bool exists() const;

        bool isDirectory() const;

        bool isHidden() const;

        bool isFile() const;

        bool isSymLink() const;

        bool isDotDirectory() const;

        bool canRead() const;

        bool canWrite() const;

        bool canExecute() const;

        void list(PathArray& dest, bool sortByDirectory = false) const;

        void list(StringArray& dest, bool sortByDirectory = false) const;

        Permissions permissions() const;

        DirectoryEntry entry() const;

        bool open(InputFileStream& fs, bool binary = false) const;

        bool open(OutputFileStream& fs, bool binary = false) const;

        Path append(const String& path) const;

        Path append(const Path& path) const;

        Path relativeTo(const String& to) const;

        Path relativeTo(const Path& to) const;

        const StringDeque& directories() const;

        Path absolute() const;

        String fullPlatform() const;

        static Path current();
    };

}  // namespace Rt2::Directory
