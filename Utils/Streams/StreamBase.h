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
#include <istream>
#include <ostream>
#include "Utils/Allocator.h"
#include "Utils/Array.h"
#include "Utils/Definitions.h"
#include "Utils/String.h"

namespace Rt2
{
    namespace Stream
    {
        using BufferType = SimpleArray<char, Allocator<char, size_t>>;
        using int_type   = std::streambuf::int_type;  // int
        using streamsize = std::streamsize;           // long long

        constexpr size_t Extra = 0x10;

    }  // namespace Stream

    class OutputBufferStream final : public std::ostream
    {
    public:
        using PointerType      = Stream::BufferType::BaseType::PointerType;
        using ConstPointerType = Stream::BufferType::BaseType::ConstPointerType;

    private:
        class StreamBuffer final : public std::streambuf
        {
        private:
            Stream::BufferType _buffer;

            Stream::int_type put(const char ch)
            {
                _buffer.push_back(ch);
                return 1;
            }

            Stream::int_type write(const char* ptr, const size_t& count)
            {
                RT_GUARD_CHECK_RET(ptr, -1)

                const size_t n = count;
                const size_t c = _buffer.capacity();
                const size_t s = _buffer.size();

                if (const auto ns = s + n; ns >= c)
                    _buffer.reserve(c + n + Stream::Extra);

                const auto end = _buffer._end();
                Fill(end, ptr, n);

                _buffer.resize(_buffer.size() + n);
                return (int)(n);
            }

            void clear()
            {
                _buffer.resizeFast(0);
            }

        public:
            StreamBuffer() = default;

            void reserve(const size_t& space)
            {
                _buffer.reserve(space);
            }

            void string(String& str)
            {
                str = {
                    _buffer.data(),
                    _buffer.size(),
                };
            }

            void reset()
            {
                _buffer.resizeFast(0);
            }

            ConstPointerType data() const
            {
                return _buffer.data();
            }

            size_t size() const
            {
                return _buffer.size();
            }

        protected:
            int_type overflow(const int_type v) override
            {
                return put((char)v);
            }

            std::streamsize xsputn(const char* ptr, const std::streamsize count) override
            {
                return write(ptr, count);
            }
        };
        StreamBuffer _buffer;

    public:
        OutputBufferStream() :
            std::ostream(&_buffer)
        {
        }

        void reserve(const size_t& space)
        {
            _buffer.reserve(space);
        }

        void string(String& str)
        {
            _buffer.string(str);
        }

        String string()
        {
            String copy;
            _buffer.string(copy);
            return copy;
        }

        void copy(IStream& in)
        {
            const pos_type cp = in.tellg();
            in.seekg(0, end);
            const std::streamsize sz = in.tellg();
            in.seekg(0, beg);

            if (sz > 0)
            {
                Stream::BufferType buf;
                buf.reserve(sz);
                in.read(buf.data(), sz);
                write(buf.data(), sz);
            }
            in.seekg(cp, cur);
        }

        template <typename... Args>
        void println(Args&&... args)
        {
            OStream& os = *this;
            ((os << std::forward<Args>(args)), ...);
            os << std::endl;
        }

        template <typename... Args>
        void print(Args&&... args)
        {
            OStream& os = *this;
            ((os << std::forward<Args>(args)), ...);
        }

        void reset()
        {
            _buffer.reset();
        }

        ConstPointerType data() const
        {
            return _buffer.data();
        }

        size_t size() const
        {
            return _buffer.size();
        }
    };

    class InputBufferStream final : public std::istream
    {
    public:
        using PointerType      = Stream::BufferType::BaseType::PointerType;
        using ConstPointerType = Stream::BufferType::BaseType::ConstPointerType;

    private:
        class StreamBuffer final : public std::streambuf
        {
        private:
            Stream::BufferType _buffer;
            PointerType        _begin{nullptr};
            PointerType        _end{nullptr};
            std::streamsize    _total{0};

            void clear()
            {
                _buffer.resizeFast(0);
            }

        public:
            StreamBuffer() = default;

            explicit StreamBuffer(const String& value)
            {
                _buffer.write(value.data(), value.size());
                _begin = _buffer.begin();
                _end   = _buffer.end();
                _total = (std::streamsize)value.size();
            }

            explicit StreamBuffer(const OutputBufferStream& value)
            {
                _buffer.write(value.data(), value.size());
                _begin = _buffer.begin();
                _end   = _buffer.end();
                _total = (std::streamsize)value.size();
            }

            void reserve(const size_t& space)
            {
                _buffer.reserve(space);
            }

            void string(String& str)
            {
                str = {
                    _buffer.data(),
                    _buffer.size(),
                };
            }

            void reset()
            {
                _buffer.resizeFast(0);
            }

            Stream::BufferType& iBuf() { return _buffer; }

        protected:
            std::streamsize showmanyc() override
            {
                return _end - _begin;
            }

            int_type next() const
            {
                if (_buffer.empty())
                    return traits_type::eof();
                if (_begin == _end)
                    return traits_type::eof();
                return traits_type::to_int_type(*_begin);
            }

            int_type underflow() override
            {
                return next();
            }

            int_type uflow() override
            {
                const int_type rc = next();
                if (rc > 0) ++_begin;
                return rc;
            }

            int_type pbackfail(int_type) override
            {
                if (_total > 0 && _begin)
                {
                    _total -= 1;
                    --_begin;
                    return 1;
                }
                return traits_type::eof();
            }
        };

        StreamBuffer _buffer;

    public:
        InputBufferStream() :
            std::istream(&_buffer)
        {
        }

        explicit InputBufferStream(const String& value) :
            std::istream(&_buffer),
            _buffer{value}
        {
        }

        explicit InputBufferStream(const OutputBufferStream& value) :
            std::istream(&_buffer),
            _buffer{value}
        {
        }

        void reserve(const size_t& space)
        {
            _buffer.reserve(space);
        }

        void string(String& str)
        {
            _buffer.string(str);
        }

        String string()
        {
            String copy;
            _buffer.string(copy);
            return copy;
        }

        void copy(IStream& in)
        {
            const pos_type cp = in.tellg();
            in.seekg(0, end);
            const std::streamsize sz = in.tellg();
            in.seekg(0, beg);

            if (sz > 0)
            {
                Stream::BufferType buf;
                buf.reserve(sz);
                in.read(buf.data(), sz);
                _buffer.iBuf().write(buf.data(), sz);
            }
            in.seekg(cp, cur);
        }

        template <typename... Args>
        void get(Args&&... args)
        {
            IStream& os = *this;
            ((os >> std::forward<Args>(args)), ...);
        }

        void reset()
        {
            _buffer.reset();
        }
    };

}  // namespace Rt2
