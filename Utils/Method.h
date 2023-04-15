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

namespace Rt2
{
    template <typename T>
    class VoidMethod
    {
    public:
        using Instance = T;
        typedef void (Instance::*Invoke)();

    private:
        Invoke    _method{nullptr};
        Instance* _self{nullptr};

    public:
        VoidMethod() = default;

        void bind(Instance* self, Invoke ot)
        {
            _self   = self;
            _method = ot;
        }

        void operator()()
        {
            if (_self && _method)
                (_self->*_method)();
        }
    };

    template <typename T, typename Arg>
    class Method
    {
    public:
        using Instance = T;
        typedef void (Instance::*Invoke)(const Arg&);

    private:
        Invoke    _method{nullptr};
        Instance* _self{nullptr};

    public:
        Method()                  = default;
        Method(const Method& oth) = delete;

        Method(Instance* self, const Invoke& ot) :
            _method(ot),
            _self(self)
        {
        }

        void bind(Instance* self, Invoke ot)
        {
            _self   = self;
            _method = ot;
        }

        void operator()(const Arg& arg)
        {
            if (_self && _method)
                (_self->*_method)(arg);
        }
    };

}  // namespace Rt2
