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

#ifdef _WIN32
    #include <filesystem>
    #define StdFileSystem std::filesystem
#else
    #include <experimental/filesystem>
    #define StdFileSystem std::experimental::filesystem
#endif

#include "Utils/String.h"

namespace Rt2
{
    using FilePath          = StdFileSystem::path;
    using DirectoryEntry    = StdFileSystem::directory_entry;
    using DirectoryIterator = StdFileSystem::directory_iterator;
    using Permissions       = StdFileSystem::perms;
    using PathArray           = std::deque<FilePath>;
    using DirectoryEntryArray = std::deque<DirectoryEntry>;

    enum ExtraPerm
    {
        EP_NONE   = 0x00,
        EP_HIDDEN = 0x01,
        EP_SYSTEM = 0x02,
        EP_DOT    = 0x03,
    };


    class FileSystem
    {
    public:
        //[[deprecated]]
        static String normalize(const String& path);

        //[[deprecated]]
        static FilePath normalize(const FilePath& path);

        //[[deprecated]]
        static FilePath absolute(const String& input);

        //[[deprecated]]
        static String currentPath();

        static String current();

        static bool isRooted(const String& test);

        static DirectoryIterator tryGet(const DirectoryEntry& ent);

        static ExtraPerm access(const DirectoryEntry& ent);

        static bool isFile(const DirectoryEntry& ent);

        static bool isDirectory(const DirectoryEntry& ent);

        static size_t fileSize(const DirectoryEntry& ent);

        static void list(const DirectoryEntry& root,
                         DirectoryEntryArray*  directories   = nullptr,
                         DirectoryEntryArray*  files         = nullptr,
                         size_t*               totalFileSize = nullptr);

        static String sanitize(const String& path);

        static String sanitizePlatform(const String& path);

        static String unixPath(const String& path);

        static FilePath unixPath(const FilePath& path);

        static void list(const String& path, DirectoryEntryArray& dest);
    };

    using Fs = FileSystem;

}  // namespace Rt2
