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
#include "Utils/Definitions.h"

#define RT_DECLARE_TYPE(T)               \
    using ValueType          = T;        \
    using ReferenceType      = T&;       \
    using PointerType        = T*;       \
    using ConstValueType     = const T;  \
    using ConstPointerType   = const T*; \
    using ConstReferenceType = const T&;

#define RT_DECLARE_NAMED_TYPE(T)         \
    using T##ValueType          = T;        \
    using T##ReferenceType      = T&;       \
    using T##PointerType        = T*;       \
    using T##ConstValueType     = const T;  \
    using T##ConstPointerType   = const T*; \
    using T##ConstReferenceType = const T&;

#define RT_DECLARE_REF_TYPE(T)                               \
    typedef typename T::ValueType          ValueType;        \
    typedef typename T::ReferenceType      ReferenceType;    \
    typedef typename T::PointerType        PointerType;      \
    typedef typename T::ConstValueType     ConstValueType;   \
    typedef typename T::ConstPointerType   ConstPointerType; \
    typedef typename T::ConstReferenceType ConstReferenceType;
