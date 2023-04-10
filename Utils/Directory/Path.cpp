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
        // this caching is probably more harm than good!
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

    enum Flags
    {
        PathNoFlag       = 0x00,
        PathHasRoot      = 0x01,
        PathHasExtension = 0x02,
        PathIsOnlyDir    = 0x03,
    };

    enum State
    {
        SearchPeriod,  // Search until the EOS or the first '/' character
        SearchSep,     // Search until the EOS or the first ':' character
        SearchDrive,   // Search until the EOS
    };

    struct PathPrivate
    {
        String      root;
        StringDeque directories;
        String      stem;
        StringDeque extensions;
        PathCache   cache;
        uint32_t    flags{PathNoFlag};

        void clear()
        {
            root.clear();
            directories.clear();
            stem.clear();
            extensions.clear();
            cache.clear();
            flags = PathNoFlag;
        }

        bool empty() const
        {
            return root.empty() &&
                   directories.empty() &&
                   stem.empty() &&
                   extensions.empty();
        }

        static void scan(IStream& input, String& out, int& state);

        void constructImpl(const String& str);

        void construct(const String& str);

        static void pushFlipped(StringDeque& dest, const String& str, char ex = 0);
    };

    void PathPrivate::scan(IStream& input, String& out, int& state)
    {
        out.clear();
        while (!input.eof())
        {
            const int ch = input.get();
            if (ch <= 0)
                break;

            switch (ch)
            {
            case '/':
                state = SearchSep;
                if (input.peek() == '/')
                    (void)input.get();
                return;
            case '.':
                if (state == SearchPeriod)
                    return;
                [[fallthrough]];
            case PathSpecialChars:
            case UpperCaseAz:
            case LowerCaseAz:
            case Digits09:
                out.push_back((char)ch);
                break;
            case ':':
                state = SearchDrive;
                break;
            default:
                throw Exception("Invalid character in path sequence :", Hex(ch));
            }
        }
    }

    void PathPrivate::constructImpl(const String& str)
    {
        clear();

        String n2 = FileSystem::sanitize(str);
        if (Su::startsWith(n2, "./"))
            n2 = Su::join(FileSystem::current(), n2.substr(2, n2.size()));

        if (Su::startsWith(n2, '/') || FileSystem::isRooted(n2))
            flags |= PathHasRoot;

        String n3;
        Su::reverse(n3, n2);  // read it backwards

        int state = SearchPeriod;
        int prev  = Su::startsWith(n3, '/') ? SearchSep : SearchPeriod;
        if (prev)
            flags |= PathIsOnlyDir;

        String t0;

        InputStringStream stream(n3);
        while (!stream.eof())
        {
            scan(stream, t0, state);
            if (prev == state)  // no transition
            {
                if (state == SearchPeriod)
                {
                    if (stream.eof())
                        Su::reverse(stem, t0);
                    else
                        pushFlipped(extensions, t0);
                }
                else if (state == SearchSep)
                    pushFlipped(directories, t0, '/');
                else
                    Console::writeError("unhandled exit: ", state);
            }
            else  // transition state
            {
                if (prev == SearchPeriod)
                    Su::reverse(stem, t0);
                else if (prev == SearchSep && state == SearchDrive)
                    Su::reverse(root, t0);
                else
                    Console::writeError("unhandled transition: ", prev, "->", state);
            }
            prev = state;
        }

        if (!root.empty())
            flags |= PathHasRoot;
        if (!extensions.empty())
            flags |= PathHasExtension;
    }

    void PathPrivate::construct(const String& str)
    {
        try
        {
            if (!str.empty())
                constructImpl(str);
        }
        catch (Exception& ex)
        {
            Console::writeError(ex.what());
            clear();
        }
    }

    void PathPrivate::pushFlipped(StringDeque& dest, const String& str, char ex)
    {
        if (str.empty())
            return;

        if (str.size() > 1)
        {
            String tmp;
            Su::reverse(tmp, str);
            dest.push_front(tmp);
        }
        else
            dest.push_front(str);

        if (ex != 0)
            dest.front().push_back(ex);
    }

    Path::Path() :
        _private(new PathPrivate())
    {
    }

    Path::Path(const Path& oth) :
        _private(new PathPrivate())
    {
        _private->construct(oth.full());
    }

    Path::Path(const String& fileName) :
        _private(new PathPrivate())

    {
        _private->construct(fileName);
    }

    Path::~Path()
    {
        delete _private;
        _private = nullptr;
    }

    Path& Path::operator=(const Path& oth)
    {
        if (this != &oth)
            _private->construct(oth.full());
        return *this;
    }

    Path& Path::operator=(const String& oth)
    {
        _private->construct(oth);
        return *this;
    }

    bool Path::empty() const
    {
        return _private->empty();
    }

    const String& Path::root() const
    {
        if (!empty() && _private->cache.drive.empty())
        {
            if (isRooted())
            {
                if (_private->root.empty())
                    _private->cache.drive = "/";
                else
                    _private->cache.drive = _private->root + ":/";
            }
        }
        return _private->cache.drive;
    }

    const String& Path::directory() const
    {
        if (!empty() && _private->cache.directory.empty())
        {
            if (!_private->directories.empty())
                Su::combine(_private->cache.directory, _private->directories, 0, 0);
        }
        return _private->cache.directory;
    }

    String Path::lastDirectory() const
    {
        String dir;
        if (!_private->directories.empty())
            dir = Su::join(_private->directories.back(), '/');
        return dir;
    }

    const String& Path::extension() const
    {
        if (!empty() && _private->cache.extension.empty())
        {
            if (!_private->extensions.empty())
                Su::combine(_private->cache.extension, _private->extensions, '.');
        }
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
        if (!empty() && _private->cache.full.empty())
            _private->cache.full = Su::join(root(), directory(), stem(), extension());
        return _private->cache.full;
    }

    const String& Path::fullDirectory() const
    {
        if (!empty() && _private->cache.fullDirectory.empty())
            _private->cache.fullDirectory = Su::join(root(), directory());
        return _private->cache.fullDirectory;
    }

    Path Path::directoryAsPath() const
    {
        return Path{fullDirectory()};
    }

    bool Path::exists() const
    {
        try
        {
            return StdFileSystem::exists(FilePath(full()));
        }
        catch (...)
        {
            return false;
        }
    }

    bool Path::isDirectory() const
    {
        try
        {
            return StdFileSystem::is_directory(FilePath(full()));
        }
        catch (...)
        {
            return false;
        }
    }

    bool Path::isHidden() const
    {
        try
        {
            return isDotDirectory();
        }
        catch (...)
        {
            return true;
        }
    }

    bool Path::isFile() const
    {
        try
        {
            return StdFileSystem::is_regular_file(FilePath(full()));
        }
        catch (...)
        {
            return false;
        }
    }

    bool Path::isSymLink() const
    {
        try
        {
            return StdFileSystem::is_symlink(FilePath(full()));
        }
        catch (...)
        {
            return true;
        }
    }

    bool Path::isDotDirectory() const
    {
        return Su::startsWith(lastDirectory(), '.') || Su::startsWith(base(), '.');
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

    void Path::list(PathArray& dest, const bool sortByDirectory) const
    {
        if (isSymLink() || !canRead())
            return;

        try
        {
            if (const DirectoryEntry fp = entry();
                is_directory(fp))
            {
                const DirectoryIterator dit = DirectoryIterator(fp.path());

                for (const auto& ent : dit)
                    dest.push_back(Path(ent.path().generic_string()));
                if (sortByDirectory)
                {
                    std::sort(
                        dest.begin(),
                        dest.end(),
                        [](const Path& a, const Path& b)
                        {
                            const int av = a.isDirectory() ? 0 : 1;
                            const int bv = b.isDirectory() ? 0 : 1;
                            return av < bv;
                        });
                }
            }
        }
        catch (...)
        {
            // empty
            dest.clear();
        }
    }

    void Path::list(StringArray& dest, const bool sortByDirectory) const
    {
        PathArray pa;
        list(pa, sortByDirectory);

        for (const auto& path : pa)
            dest.push_back(path.full());
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

    bool Path::open(OutputFileStream& fs, const bool binary) const
    {
        if (binary)
            fs.open(full(), std::ios_base::binary);
        else
            fs.open(full());
        return fs.is_open();
    }

    Path Path::append(const String& path) const
    {
        if (isDirectory())
        {
            return Path(Su::join(fullDirectory(), path));
        }
        return {};
    }

    Path Path::append(const Path& path) const
    {
        if (isDirectory())
            return Path(Su::join(full(), path.directory(), path.base()));
        return {};
    }

    Path Path::relativeTo(const String& to) const
    {
        String np;
        Su::replaceAll(np, full(), to, "");
        return Path(np);
    }

    Path Path::relativeTo(const Path& to) const
    {
        String np;
        Su::replaceAll(np, full(), to.full(), "");
        if (isDirectory())
        {
            if (Su::startsWith(np, '/'))
                np = np.substr(1, np.size());
            np = Su::join(np, "/");
        }
        return Path(np);
    }

    const StringDeque& Path::directories() const
    {
        return _private->directories;
    }

    Path Path::absolute() const
    {
        if (_private->root.empty())
        {
            return Path(
                Su::join(
                    FileSystem::current(),
                    directory(),
                    stem(),
                    extension()));
        }
        return Path(full());
    }

    String Path::fullPlatform() const
    {
#ifdef WIN32
        String fix;
        Su::replaceAll(fix, full(), "/", "\\");
        return fix;
#else
        return full();
#endif
    }

    Path Path::current()
    {
        return Path(FileSystem::current());
    }

    Path Path::combine(const String& a, const String& b)
    {
        String val = a;
        if (!Su::endsWith(val, '/'))
            val.push_back('/');
        return Path(Su::join(val, b));
    }

    Path Path::parentDir() const
    {
        if (!stem().empty())
            return Path(fullDirectory());

        StringDeque dirs = directories();
        if (!dirs.empty())
            dirs.pop_back();

        if (!dirs.empty())
        {
            String np;
            Su::combine(np, dirs, 0, '/');
            return Path(Su::join(root(), np));
        }
        return Path(Su::join(root(), '/'));
    }

    const String& Path::stem() const
    {
        return _private->stem;
    }

    bool Path::hasDirectory() const
    {
        return !_private->directories.empty();
    }

    bool Path::isRooted() const
    {
        return (_private->flags & PathHasRoot) != 0;
    }

    const String& Path::base() const
    {
        if (!empty() && _private->cache.base.empty())
            _private->cache.base = Su::join(stem(), extension());
        return _private->cache.base;
    }
}  // namespace Rt2::Directory
