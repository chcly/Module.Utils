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
#include "Utils/FileSystem.h"
#include "Utils/Char.h"

namespace Rt2
{
    String FileSystem::normalize(const String& path)
    {
#ifdef _WIN32
        const String sep = "\\";
        const String swp = "/";
#else
        const String sep = "/";
        const String swp = "\\";
#endif
        String cp1;
        Su::trimWs(cp1, path);

        String cp2;
        Su::replaceAll(cp2, cp1, swp, sep);
        return cp2;
    }

    FilePath FileSystem::normalize(const FilePath& path)
    {
        return FilePath{normalize(path.string())};
    }

    FilePath FileSystem::absolute(const String& input)
    {
        const FilePath value(input);

        if (!value.has_root_directory())
        {
            const String current = currentPath();
            return normalize(Su::join(
                current,
                '/',
                input));
        }
        return normalize(value);
    }

    String FileSystem::currentPath()
    {
        return StdFileSystem::current_path().string();
    }

    String FileSystem::current()
    {
        String path = sanitize(StdFileSystem::current_path().string());
        if (!Su::endsWith(path, '/'))
            path.push_back('/');
        return path;
    }

    bool FileSystem::isRooted(const String& test)
    {
        return FilePath(test).has_root_directory();
    }

    DirectoryIterator FileSystem::tryGet(const DirectoryEntry& ent)
    {
        try
        {
            return DirectoryIterator(ent);
        }
        catch (...)
        {
            // treat it like it does not exist.
            return {};
        }
    }

    void FileSystem::list(const DirectoryEntry& root,
                          DirectoryEntryArray*  directories,
                          DirectoryEntryArray*  files,
                          size_t*               totalFileSize)
    {
        if (!directories && !files && !totalFileSize)
            return;

        const auto it = tryGet(root);

        for (const auto& ent : it)
        {
            if (is_directory(ent))
            {
                if (directories)
                    directories->push_front(ent);
            }
            else
            {
                if (files)
                    files->push_front(ent);

                if (totalFileSize)
                    (*totalFileSize) += ent.file_size();
            }
        }
    }

    String FileSystem::sanitize(const String& path)
    {
        // only dealing with Unix path separators.
        String n1;
        Su::replaceAll(n1, path, "\\", "/");

        // Take double slashes out of the equation
        String n2;
        Su::replaceAll(n2, n1, "//", "/");
        return n2;
    }

    String FileSystem::unixPath(const String& path)
    {
        String cp1;
        Su::trimWs(cp1, path);

        String cp2;
        Su::replaceAll(cp2, cp1, "\\\\", "\\");
        Su::replaceAll(cp2, cp1, "\\", "/");
        return cp2;
    }

    FilePath FileSystem::unixPath(const FilePath& path)
    {
        return FilePath{unixPath(path.string())};
    }

    void FileSystem::list(
        const String&        path,
        DirectoryEntryArray& dest)
    {
        if (const FilePath fp = {path};
            is_directory(fp))
        {
            const DirectoryIterator it{fp};
            for (const auto& val : it)
            {
                if (is_directory(val))
                    dest.push_front(val);
                else
                    dest.push_back(val);
            }
        }
    }
}  // namespace Rt2
