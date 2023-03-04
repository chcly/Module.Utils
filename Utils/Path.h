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
    class PathUtil
    {
    private:
        String      _root;
        String      _directory;
        String      _file;
        StringDeque _extension;

        void construct(const String& str);

    public:
        [[deprecated]]
        PathUtil() = default;

        [[deprecated]]
        PathUtil(const PathUtil& oth);

        [[deprecated]]
        explicit PathUtil(const String& fileName);

        const String& stem() const;

        const String& directory() const;

        void directoryList(StringDeque& dir) const;

        bool hasDirectory() const;

        String parentDir(size_t n) const;

        PathUtil parentPath(size_t n) const;

        bool exists() const;

        String fullPath() const;

        String rootedDir() const;

        String fileName() const;

        void fileName(const PathUtil& file);

        void fileName(const String& file);

        String fullExtension() const;

        String lastExtension() const;

        String firstExtension() const;

        void appendDirectory(const PathUtil& filePath);

        void appendDirectory(const String& filePath);

        bool empty() const;

        bool isAbsolute() const;

        bool hasExtensions() const;

        static PathUtil construct(const String& path, const String& srcPath);
    };

    inline bool PathUtil::empty() const
    {
        return _file.empty();
    }

}  // namespace Rt2
