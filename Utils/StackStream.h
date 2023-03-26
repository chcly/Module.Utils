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
#include <iomanip>
#include "Utils/Stack.h"
#include "Utils/String.h"
#include "Utils/TextStreamWriter.h"

namespace Rt2
{
    template <int Width = 2, int Max = 80, int MaxLevel = 20>
    class TStreamStack
    {
    public:
        static constexpr int IndentWidth = Width;
        static constexpr int IndentMax   = Max;
        static constexpr int LevelMax    = MaxLevel;

        struct StackObject
        {
            OStream*            client{nullptr};
            OutputStringStream* stream{nullptr};
            int                 indent{0};
        };
        using StackStream = Stack<StackObject>;

    private:
        static void flush(const String& buffer, OStream* dest)
        {
            if (dest)
            {
                dest->write(buffer.c_str(), (std::streamsize)buffer.size());
                dest->flush();
            }
        }

    public:
        class Flush
        {
        private:
            TStreamStack* _parent{nullptr};

            Flush() = default;

            Flush& operator=(const Flush& rhs) = default;

        public:
            Flush(const Flush& rhs) = default;

            explicit Flush(TStreamStack* base)
            {
                _parent = base;
            }

            ~Flush()
            {
                if (_parent)
                    _parent->flush();
            }
        };

    private:
        StackStream _stack;
        int         _depth{0};
        bool        _stickyLine{false};
        OStream*    _root{nullptr};

        StackObject top()
        {
            if (!_stack.empty())
                return _stack.top();
            return {};
        }

    public:
        TStreamStack() = default;

        ~TStreamStack()
        {
            flush();
        }

        void flush()
        {
            while (!_stack.empty())
                pop();
        }

        void bind(OStream* base)
        {
            _root = base;
        }

        void inc(const int n)
        {
            if (n != 0)
                _depth += Clamp(n, -LevelMax, LevelMax);
        }

        [[nodiscard]] Flush push(OStream* stream)
        {
            if (stream)
            {
                _stack.push({stream, new OutputStringStream(), _depth});
                return Flush{this};
            }
            return Flush{nullptr};
        }

        void _push(OStream* stream)
        {
            // Note: If you do something like
            //
            // TStreamStack<4> stream;
            // OutputStringStream rhs;
            // stream.push(&rhs);
            //
            // This is fine... but be aware of scope and ownership.
            // (Flush is trying to do that automatically)

            if (stream)
                _stack.push({stream, new OutputStringStream(), _depth});
        }

        void pop()
        {
            if (!_stack.empty())
            {
                const auto [client, stream, indent] = _stack.top();

                _depth = indent;

                const String buf = stream ? stream->str() : "";
                delete stream;

                flush(buf, client);
                if (_root != client)
                    flush(buf, _root);

                _stack.pop();
            }
        }

        void endl()
        {
            if (auto [client, stream, indent] = top();
                stream != nullptr)
            {
                *stream << std::endl;
                _stickyLine = false;
            }
        }

        template <typename... Args>
        void print(Args&&... args)
        {
            if (auto [client, stream, indent] = top();
                stream != nullptr)
            {
                if (!_stickyLine)
                {
                    Ts::indent(
                        *stream,
                        Clamp<int>(_depth * IndentWidth, 0, 80));
                    _stickyLine = true;
                }
                ((*stream << std::forward<Args>(args)), ...);
            }
        }

        template <typename... Args>
        void println(Args&&... args)
        {
            if (auto [client, stream, indent] = top();
                stream != nullptr)
            {
                if (!_stickyLine)
                {
                    Ts::indent(
                        *stream,
                        Clamp<int>(_depth * IndentWidth, 0, 80));
                }
                ((*stream << std::forward<Args>(args)), ...);
                *stream << std::endl;
                _stickyLine = false;
            }
        }
    };

    using OutputStreamStack = TStreamStack<4>;
    using StackFlush        = OutputStreamStack::Flush;

}  // namespace Rt2
