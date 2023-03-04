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
#include "Utils/Directory/Path.h"
#include "Utils/Char.h"
#include "Utils/Exception.h"
#include "Utils/FileSystem.h"
#include "Utils/StreamMethods.h"

namespace Rt2::Directory
{

    struct PathCache
    {
        String drive;
        String directory;
        String extension;
        String base;
        String full;
        String fullDirectory;

        void clear()
        {
            drive.clear();
            directory.clear();
            extension.clear();
            base.clear();
            full.clear();
            fullDirectory.clear();
        }
    };

    struct PathPrivate
    {
        String      drive;
        StringDeque directories;
        String      stem;
        StringDeque extensions;
        PathCache   cache;

        void clear()
        {
            drive.clear();
            directories.clear();
            stem.clear();
            extensions.clear();
            cache.clear();
        }
    };

    int Path::scan(IStream& input, String& out, int& state)
    {
        // state: 0 - file
        // state: 1 - dir

        out.clear();
        while (!input.eof())
        {
            const int ch = input.get();
            if (ch <= 0)
                break;

            switch (ch)
            {
            case '/':
                state = 1;
                if (input.peek() == '/')
                    throw Exception("Malformed path");
                return 1;
            case '.':
                return 1;

            case PathSpecialChars:
            case UpperCaseAz:
            case LowerCaseAz:
            case Digits09:
                out.push_back((char)ch);
                break;
            case ':':
                state = 2;
                break;
            default:
                throw Exception("Invalid character in path sequence :", Hex(ch));
            }
        }
        return -1;
    }

    void Path::construct(const String& str) const
    {
        RT_ASSERT(_private)
        _private->clear();

        // only dealing with unix path separators.
        String norm;
        Su::replaceAll(norm, str, "\\", "/");

        String rn;
        Su::reverse(rn, norm);  // read it backwards

        int state = 0;
        int prev  = 0;

        String flip;
        String buf;

        InputStringStream stream(rn);
        while (!stream.eof())
        {
            scan(stream, buf, state);
            if (prev == state)
            {
                if (state == 0)
                {
                    Su::reverse(flip, buf);
                    _private->extensions.push_front(flip);
                }
                else if (state == 1)
                {
                    Su::reverse(flip, buf);
                    _private->directories.push_front(flip);
                }
            }
            else
            {
                if (prev == 0)
                    Su::reverse(_private->stem, buf);
                if (prev == 1 && state == 2)
                    Su::reverse(_private->drive, buf);
            }
            prev = state;
        }
    }

    Path::Path() :
        _private(new PathPrivate())
    {
    }

    Path::Path(const Path& oth) :
        _private(new PathPrivate())
    {
        construct(oth.full());
    }

    Path::Path(const String& fileName) :
        _private(new PathPrivate())

    {
        construct(fileName);
    }

    Path::~Path()
    {
        delete _private;
        _private = nullptr;
    }

    Path& Path::operator=(const Path& oth)
    {
        if (this != &oth)
            construct(oth.full());
        return *this;
    }

    Path& Path::operator=(const String& oth)
    {
        construct(oth);
        return *this;
    }

    const String& Path::root() const
    {
        if (_private->cache.drive.empty())
        {
            if (hasDirectory())
            {
                if (_private->drive.empty())
                    _private->cache.drive = "/";
                else
                    _private->cache.drive = _private->drive + ":/";
            }
        }
        return _private->cache.drive;
    }

    const String& Path::directory() const
    {
        if (_private->cache.directory.empty())
            Su::combine(_private->cache.directory, _private->directories, 0, '/');
        return _private->cache.directory;
    }

    const String& Path::extension() const
    {
        if (_private->cache.extension.empty())
            Su::combine(_private->cache.extension, _private->extensions, '.');
        return _private->cache.extension;
    }

    String Path::firstExtension() const
    {
        String ret;
        if (!_private->extensions.empty())
        {
            ret.push_back('.');
            ret.append(_private->extensions.front());
        }
        return ret;
    }

    String Path::lastExtension() const
    {
        String ret;
        if (!_private->extensions.empty())
        {
            ret.push_back('.');
            ret.append(_private->extensions.back());
        }
        return ret;
    }

    const String& Path::full() const
    {
        if (_private->cache.full.empty())
            _private->cache.full = Su::join(root(), directory(), stem(), extension());
        return _private->cache.full;
    }

    const String& Path::fullDirectory() const
    {
        if (_private->cache.fullDirectory.empty())
            _private->cache.fullDirectory = Su::join(root(), directory());
        return _private->cache.fullDirectory;
    }

    bool Path::exists() const
    {
        return StdFileSystem::exists(FilePath(full()));
    }

    bool Path::isDirectory() const
    {
        return StdFileSystem::is_directory(FilePath(full()));
    }

    bool Path::isFile() const
    {
        return StdFileSystem::is_regular_file(FilePath(full()));
    }

    bool Path::isSymLink() const
    {
        return StdFileSystem::is_symlink(FilePath(full()));
    }

    void Path::list(PathArray& dest) const
    {
        const DirectoryIterator dit = DirectoryIterator(FilePath(full()));
        for (const auto& ent : dit)
            dest.push_back(Path(ent.path().generic_string()));
    }

    Permissions Path::permissions() const
    {
        return entry().status().permissions();
    }

    DirectoryEntry Path::entry() const
    {
        return DirectoryEntry(FilePath(full()));
    }

    bool Path::open(InputFileStream& fs, const bool binary) const
    {
        if (isFile())
        {
            if (binary)
                fs.open(full(), std::ios_base::binary);
            else
                fs.open(full());
        }
        return fs.is_open();
    }

    bool Path::canRead() const
    {
        return (permissions() & Permissions::owner_read) != Permissions::none;
    }

    bool Path::canWrite() const
    {
        return (permissions() & Permissions::owner_write) != Permissions::none;
    }

    bool Path::canExecute() const
    {
        return (permissions() & Permissions::owner_exec) != Permissions::none;
    }

    const String& Path::stem() const
    {
        return _private->stem;
    }

    bool Path::hasDirectory() const
    {
        return !_private->directories.empty();
    }

    const String& Path::base() const
    {
        if (_private->cache.base.empty())
            _private->cache.base = Su::join(stem(), extension());
        return _private->cache.base;
    }
}  // namespace Rt2::Directory
