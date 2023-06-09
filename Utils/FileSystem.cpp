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
#include "Console.h"
#ifdef _WIN32
    #include <corecrt_io.h>
#endif

namespace Rt2
{
    using FileType   = StdFileSystem::file_type;
    using DirOptions = StdFileSystem::directory_options;

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
        try
        {
            return StdFileSystem::current_path().string();
        }
        catch (StdFileSystem::filesystem_error& ex)
        {
            Console::println(ex.what());
            return {};
        }
        catch (...)
        {
            return {};
        }
    }

    String FileSystem::current()
    {
        String path = sanitize(currentPath());
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
            if (!isDirectory(ent))
                return {};
            return {ent, DirOptions::skip_permission_denied};
        }
        catch (StdFileSystem::filesystem_error& ex)
        {
            Console::println(ex.what());
            return {};
        }
        catch (...)
        {
            return {};
        }
    }

    void portableHack(const FilePath& filePath, int& p)
    {
#ifdef _WIN32
        // opinion: permissions for system files should
        // be abstracted to 0000, or 0700
        p = EP_NONE;
        if (Su::startsWith(filePath.stem().generic_string(), '.'))
        {
            p |= EP_DOT | EP_HIDDEN;
        }

        _finddata_t find = {};
        if (const intptr_t fp = _findfirst(filePath.generic_string().c_str(), &find);
            fp != -1)
        {
            if (find.attrib & _A_SYSTEM)
                p |= EP_SYSTEM;
            if (find.attrib & _A_HIDDEN)
                p |= EP_HIDDEN;
            _findclose(fp);
        }
#else

        p = EP_NONE;

        if (Su::startsWith(filePath.stem().generic_string(), '.'))
        {
            p |= EP_DOT | EP_HIDDEN;
        }
#endif
    }

    ExtraPerm FileSystem::access(const DirectoryEntry& ent)
    {
        int ph;
        portableHack(ent.path(), ph);
        return (ExtraPerm)ph;
    }

    bool FileSystem::isFile(const DirectoryEntry& ent)
    {
        return ent.status().type() == FileType::regular;
    }

    bool FileSystem::isFile(const String& ent)
    {
        return isFile(DirectoryEntry(ent));
    }

    bool FileSystem::isDirectory(const DirectoryEntry& ent)
    {
        return ent.status().type() == FileType::directory;
    }

    bool FileSystem::isDirectory(const String& ent)
    {
        return isDirectory(DirectoryEntry(ent));
    }

    size_t FileSystem::fileSize(const DirectoryEntry& ent)
    {
        try
        {
            return StdFileSystem::file_size(ent);
        }
        catch (StdFileSystem::filesystem_error& ex)
        {
            Console::println(ex.what());
            return 0;
        }
        catch (...)
        {
            return 0;
        }
    }

    String FileSystem::extension(const DirectoryEntry& ent)
    {
        return ent.path().extension().generic_string();
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
            if (access(ent) != EP_NONE)
                continue;

            if (isDirectory(ent))
            {
                if (directories)
                    directories->push_front(ent);
            }
            else
            {
                if (files)
                    files->push_front(ent);

                if (totalFileSize)
                    (*totalFileSize) += fileSize(ent);
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

    String FileSystem::sanitizePlatform(const String& path)
    {
#ifdef _WIN32
        String win;
        Su::replaceAll(win, sanitize(path), "/", "\\");
        return win;
#else
        return sanitize(path);
#endif
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
        if (const DirectoryEntry fp{FilePath(path)};
            isDirectory(fp))
        {
            const DirectoryIterator it = tryGet(fp);
            for (const auto& val : it)
            {
                if (isDirectory(val))
                    dest.push_front(val);
                else
                    dest.push_back(val);
            }
        }
    }

    void FileSystem::count(const String& path, size_t& dirs, size_t& files, size_t& size)
    {
        const auto it = tryGet(DirectoryEntry(path));

        for (const auto& ent : it)
        {
            if (access(ent) != EP_NONE)
                continue;

            if (isDirectory(ent))
                ++dirs;
            else
            {
                ++files;
                size += fileSize(ent);
            }
        }
    }
}  // namespace Rt2
