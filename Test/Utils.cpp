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
#include <cstdio>
#include "Utils/Allocator.h"
#include "Utils/Array.h"
#include "Utils/HashMap.h"
#include "gtest/gtest.h"

struct Complex
{
    Complex() = default;
    int a     = 123;
    int b     = 456;
};

GTEST_TEST(Utils, Allocator_001)
{
    try
    {
        Jam::Allocator<Complex, size_t, 32> alloc;
        alloc.allocateArray(33);
        FAIL();
    }
    catch (...)
    {
        // empty
    }
}

GTEST_TEST(Utils, Allocator_002)
{
    using Alloc = Jam::Allocator<Complex, size_t>;
    Alloc al;

    Complex* c = al.allocate();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(123, c->a);
    EXPECT_EQ(456, c->b);

    Alloc::destroy(c);
}

GTEST_TEST(Utils, Array_001)
{
    using IntArray = Jam::Array<int, Jam::AOP_SIMPLE_TYPE>;
    IntArray ia;

    // Jam::Console::writeLine((int64_t)sizeof(ia));
    // Jam::Console::writeLine((int64_t)sizeof(Jam::Allocator<int, uint32_t>));
    // Jam::Console::writeLine((int64_t)sizeof(std::vector<int>));
    ia.reserve(20000);
    for (uint32_t i = 0; i < 20000; ++i)
        ia.push_back(i);

    const uint32_t pos = ia.findBinary(2);

    EXPECT_EQ(pos, 2);
    EXPECT_EQ(ia[pos], 2);

    while (!ia.empty())
        ia.pop_back();
}

GTEST_TEST(Utils, Array_002)
{
    using IntArray = Jam::SimpleArray<int>;
    IntArray ia;

    ia.reserve(20000);

    for (uint32_t i = 0; i < 20000; ++i)
        ia.push_back(i);

    EXPECT_EQ(20001, ia.capacity());

    for (uint32_t i = 0; i < 20000; ++i)
        EXPECT_EQ(i, ia[i]);

#if JT_DEBUG == 1

    try
    {
        Jam::Console::writeLine((int64_t)ia[ia.capacity()]);
        FAIL();
    }
    catch (...)
    {
    }

    try
    {
        Jam::Console::writeLine((int64_t)ia[ia.capacity()]);
        FAIL();
    }
    catch (...)
    {
    }
#endif

    ia.clear();

    ia.push_back(1);
    ia.push_back(2);
    ia.removeOrdered(1);

    EXPECT_EQ(1, ia.front());
    EXPECT_EQ(1, ia.size());

    ia.clear();
    ia.push_back(1);
    ia.push_back(2);
    ia.removeOrdered(0);

    EXPECT_EQ(2, ia.front());
    EXPECT_EQ(1, ia.size());

    ia.clear();

    constexpr int n = 10;
    for (int i = 0; i < n; ++i)
        ia.push_back(i + 1);

    for (int s = 0; s < n; ++s)
    {
        ia.removeOrdered(0);
        int nn = (n - 1) - s;

        EXPECT_EQ(nn, ia.size());
        for (int i = 0; i < nn; ++i)
        {
            int rn = (n - nn) + (i + 1);
            EXPECT_EQ(rn, ia.at(i));
        }
    }
}

GTEST_TEST(Utils, Array_003)
{
    using IntArray = Jam::SimpleArray<int>;
    IntArray ia    = {0, 1, 2, 3, 4, 5, 6, 7};
    EXPECT_EQ(0, ia[0]);
    EXPECT_EQ(1, ia[1]);
    EXPECT_EQ(2, ia[2]);
    EXPECT_EQ(3, ia[3]);
    EXPECT_EQ(4, ia[4]);
    EXPECT_EQ(5, ia[5]);
    EXPECT_EQ(6, ia[6]);
    EXPECT_EQ(7, ia[7]);

    ia.removeOrdered(3);
    EXPECT_EQ(0, ia[0]);
    EXPECT_EQ(1, ia[1]);
    EXPECT_EQ(2, ia[2]);
    EXPECT_EQ(4, ia[3]);
    EXPECT_EQ(5, ia[4]);
    EXPECT_EQ(6, ia[5]);
    EXPECT_EQ(7, ia[6]);

    ia.removeOrdered(6);
    EXPECT_EQ(0, ia[0]);
    EXPECT_EQ(1, ia[1]);
    EXPECT_EQ(2, ia[2]);
    EXPECT_EQ(4, ia[3]);
    EXPECT_EQ(5, ia[4]);
    EXPECT_EQ(6, ia[5]);

    ia.removeOrdered(0);
    EXPECT_EQ(1, ia[0]);
    EXPECT_EQ(2, ia[1]);
    EXPECT_EQ(4, ia[2]);
    EXPECT_EQ(5, ia[3]);
    EXPECT_EQ(6, ia[4]);
}

GTEST_TEST(Utils, Array_004)
{
    using IntArray = Jam::Array<int, 2, Jam::NewAllocator<int, uint32_t, 2>>;
    try
    {
        IntArray ia = {0, 1, 2, 3};
        FAIL();
    }
    catch (...)
    {
        // empty
        // just that it throws an exception
    }
}

GTEST_TEST(Utils, HashTable_001)
{
    using IntTable = Jam::HashTable<uint32_t, void*>;
    IntTable it;
    it.reserve(0x08);
    for (uint32_t i = 0; i < 0x10001; ++i)
        it.insert(i, &it);
    for (uint32_t i = 0; i < 0xFFFF; ++i)
        EXPECT_NE(it.find(i), Jam::JtNpos32);
    EXPECT_EQ(it.capacity(), 0x20000);
}
