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

/*
 * [x] Allocator
 * [x] Array
 * [x] ArrayBase
 * [x] Char
 * [ ] Console
 * [ ] FileSystem
 * [x] FixedArray
 * [ ] FixedString
 * [ ] Hash
 * [x] HashMap
 * [ ] IndexCache
 * [ ] Path
 * [ ] Queue
 * [ ] Set
 * [x] Stack
 * [ ] String
 * [ ] StringBuilder
 * [ ] Time
 * [ ] Timer
 * [ ] Fill
 * [ ] Copy
 */

#include "Utils/Allocator.h"
#include "Utils/Array.h"
#include "Utils/Char.h"
#include "Utils/FixedArray.h"
#include "Utils/HashMap.h"
#include "Utils/Stack.h"
#include "gtest/gtest.h"

using namespace Rt2;


using IntStack = Stack<int>;

GTEST_TEST(Stack, Stack_001)
{
    IntStack a;
    EXPECT_EQ(0, a.size());
    EXPECT_EQ(0, a.topI());
    EXPECT_EQ(0, a.capacity());
    EXPECT_EQ(nullptr, a.data());
}

GTEST_TEST(Stack, Stack_002)
{
    IntStack a;
    a.reserve(16);

    EXPECT_EQ(0, a.size());
    EXPECT_EQ(0, a.topI());
    EXPECT_EQ(17, a.capacity());
    EXPECT_NE(nullptr, a.data());
}


GTEST_TEST(Stack, Stack_003)
{
    IntStack a;
    int        i;
    for (i = 0; i < 16; ++i)
    {
        a.push(i + 1);
        EXPECT_EQ(i + 1, a.top());
    }

    EXPECT_EQ(16, a.size());

    for (i = 0; i < 16; ++i)
    {
        int t = a.top();
        a.pop();
        EXPECT_EQ(16 - i, t);
    }

    EXPECT_EQ(0, a.size());
}


GTEST_TEST(Stack, Stack_004)
{
    IntStack a;
    int i;

    for (i = 0; i < 16; ++i)
        a.push(i);

    Stack b(a);

    EXPECT_EQ(a.size(), b.size());


    const IntStack::PointerType na = a.data();
    const IntStack::PointerType nb = b.data();

    for (i = 0; i < 16; ++i)
        EXPECT_EQ(na[i], nb[i]);
}

GTEST_TEST(Stack, Stack_005)
{
    IntStack a, b;
    int   i;

    for (i = 0; i < 16; ++i)
    {
        a.push(i);
        b.push(16 - i);
    }

    b = a;
    EXPECT_EQ(a.size(), b.size());


    for (i = 0; i < 16; ++i)
        EXPECT_EQ(a[i], b[i]);
}



GTEST_TEST(Utils, Char_bool)
{
    bool test = Char::toBool("Hello World");
    EXPECT_EQ(false, test);

    test = Char::toBool("true");
    EXPECT_EQ(true, test);

    test = Char::toBool("false");
    EXPECT_EQ(false, test);

    test = Char::toBool("yes");
    EXPECT_EQ(true, test);

    test = Char::toBool("no");
    EXPECT_EQ(false, test);

    test = Char::toBool("1");
    EXPECT_EQ(true, test);
}

GTEST_TEST(Utils, Char_I16)
{
    I16 test = Char::toInt16("Hello World");
    EXPECT_EQ(0, test);

    test = Char::toInt16("-32768");
    EXPECT_EQ(std::numeric_limits<I16>::min(), test);

    test = Char::toInt16("3276999");
    EXPECT_EQ(std::numeric_limits<I16>::max(), test);

    test = Char::toInt16("-987532767");
    EXPECT_EQ(std::numeric_limits<I16>::min(), test);

    test = Char::toInt16("-2147483648087655446879898769876576");
    EXPECT_EQ(std::numeric_limits<I16>::min(), test);
}

GTEST_TEST(Utils, Char_U16)
{
    U16 test = Char::toUint16("Hello World");
    EXPECT_EQ(0, test);

    test = Char::toUint16("-65536");
    EXPECT_EQ(0xFFFF, test);

    test = Char::toUint16("65536");
    EXPECT_EQ(0xFFFF, test);

    test = Char::toUint16("-987532767");
    EXPECT_EQ(0xFFFF, test);

    test = Char::toUint16("2147483648087655446879898769876576");
    EXPECT_EQ(0xFFFF, test);

    test = Char::toUint16("65534");
    EXPECT_EQ(0xFFFE, test);
}

GTEST_TEST(Utils, Char_I32)
{
    int test = Char::toInt32("Hello World");
    EXPECT_EQ(0, test);

    test = Char::toInt32("-2147483648");
    EXPECT_EQ(std::numeric_limits<int>::min(), test);

    test = Char::toInt32("2147483647");
    EXPECT_EQ(std::numeric_limits<int>::max(), test);

    test = Char::toInt32("2147483648087655446879898769876576");
    EXPECT_EQ(std::numeric_limits<int>::max(), test);

    test = Char::toInt32("-2147483648087655446879898769876576");
    EXPECT_EQ(std::numeric_limits<int>::min(), test);
}

GTEST_TEST(Utils, Char_U32)
{
    U32 test = Char::toUint32("Hello World");
    EXPECT_EQ(0, test);

    test = Char::toUint32("-4294967294");
    EXPECT_EQ(0xFFFFFFFF, test);

    test = Char::toUint32("4294967294");
    EXPECT_EQ(0xFFFFFFFE, test);

    test = Char::toUint32("2147483648087655446879898769876576");
    EXPECT_EQ(0xFFFFFFFF, test);

    test = Char::toUint32("-2147483648087655446879898769876576");
    EXPECT_EQ(0xFFFFFFFF, test);
}

GTEST_TEST(Utils, Char_I64)
{
    I64 test = Char::toInt32("Hello World");
    EXPECT_EQ(0, test);

    test = Char::toInt64("-18446744073709551616");
    EXPECT_EQ(std::numeric_limits<I64>::min(), test);

    test = Char::toInt64("18446744073709551617");
    EXPECT_EQ(std::numeric_limits<I64>::max(), test);

    test = Char::toInt64("2147483648087655446879898769876576");
    EXPECT_EQ(std::numeric_limits<I64>::max(), test);

    test = Char::toInt64("-2147483648087655446879898769876576");
    EXPECT_EQ(std::numeric_limits<I64>::min(), test);
}

GTEST_TEST(Utils, Char_U64)
{
    U64 test = Char::toUint64("Hello World");
    EXPECT_EQ(0, test);

    test = Char::toUint64("-18446744073709551616");
    EXPECT_EQ(0xFFFFFFFFFFFFFFFF, test);

    test = Char::toUint64("18446744073709551616");
    EXPECT_EQ(0xFFFFFFFFFFFFFFFF, test);

    test = Char::toUint64("18446744073709551614");
    EXPECT_EQ(0xFFFFFFFFFFFFFFFE, test);

    test = Char::toUint64("2147483648087655446879898769876576");
    EXPECT_EQ(0xFFFFFFFFFFFFFFFF, test);

    test = Char::toUint64("-2147483648087655446879898769876576");
    EXPECT_EQ(0xFFFFFFFFFFFFFFFF, test);
}


GTEST_TEST(Utils, Char_stringI16)
{
    {
        const String iString = "123";

        I16 v = Char::toInt16(iString);
        EXPECT_EQ(123, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(3, r.size());

        EXPECT_TRUE(Char::equals("123", r.c_str(), 3));
    }

    {
        const String iString = "9985287698765975976";

        I16 v = Char::toInt16(iString);
        EXPECT_EQ(32767, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(5, r.size());
        EXPECT_TRUE(Char::equals("32767", r.c_str(), 5));
    }

    {
        const String iString = "-9985287698765975976";

        I16 v = Char::toInt16(iString);
        EXPECT_EQ(-32767 - 1, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(6, r.size());
        EXPECT_TRUE(Char::equals("-32768", r.c_str(), 6));
    }
}

GTEST_TEST(Utils, Char_stringI32)
{
    {
        const String iString = "123";

        I32 v = Char::toInt32(iString);
        EXPECT_EQ(123, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(3, r.size());
        EXPECT_TRUE(Char::equals("123", r.c_str(), 3));
    }

    {
        const String iString = "9985287698765975976";

        I32 v = Char::toInt32(iString);
        EXPECT_EQ(2147483647, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(10, r.size());
        EXPECT_TRUE(Char::equals("2147483647", r.c_str(), 10));
    }

    {
        const String iString = "-9985287698765975976";

        I32 v = Char::toInt32(iString);
        EXPECT_EQ(-2147483647 - 1, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(11, r.size());
        EXPECT_TRUE(Char::equals("-2147483648", r.c_str(), 11));
    }
}

GTEST_TEST(Utils, Char_stringI64)
{
    {
        const String iString = "123";

        I64 v = Char::toInt64(iString);
        EXPECT_EQ(123, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(3, r.size());
        EXPECT_TRUE(Char::equals("123", r.c_str(), 3));
    }

    {
        const String iString = "9985287698765975988875454545476";

        I64 v = Char::toInt64(iString);
        EXPECT_EQ(9223372036854775807, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(19, r.size());
        EXPECT_TRUE(Char::equals("9223372036854775807", r.c_str(), 19));
    }

    {
        const String iString = "-9985287698765975988875454545476";

        I64 v = Char::toInt64(iString);
        EXPECT_EQ(-9223372036854775807 - 1, v);

        String r;
        Char::toString(r, v);
        EXPECT_EQ(20, r.size());
        EXPECT_TRUE(Char::equals("-9223372036854775808", r.c_str(), 20));
    }
}


GTEST_TEST(Utils, FixedArray_001)
{
    FixedArray<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(a[0], 0);
    EXPECT_EQ(a[1], 1);
    EXPECT_EQ(a[2], 2);
    EXPECT_EQ(a[3], 3);
    EXPECT_EQ(a[4], 4);
    EXPECT_EQ(a[5], 5);
    EXPECT_EQ(a[6], 6);
    EXPECT_EQ(a[7], 7);
    EXPECT_EQ(a[8], 8);
    EXPECT_EQ(a[9], 9);
    FixedArray<char, 9> b = {'h', 'e', 'l', 'l', 'o', '.', '.', '.', 0};
    EXPECT_TRUE(Rt2::Char::equals(b.ptr(), "hello...", 8));
    FixedArray<char, 9> c = b;
    EXPECT_TRUE(Rt2::Char::equals(c.ptr(), "hello...", 8));
}

GTEST_TEST(Utils, FixedArray_002)
{
    FixedArray<int, 10> a;
    for (uint16_t i = 0; i < FixedArray<int, 10>::capacity(); ++i)
        a.push_back(i);

    EXPECT_EQ(a[0], 0);
    EXPECT_EQ(a[1], 1);
    EXPECT_EQ(a[2], 2);
    EXPECT_EQ(a[3], 3);
    EXPECT_EQ(a[4], 4);
    EXPECT_EQ(a[5], 5);
    EXPECT_EQ(a[6], 6);
    EXPECT_EQ(a[7], 7);
    EXPECT_EQ(a[8], 8);
    EXPECT_EQ(a[9], 9);

    try
    {
        FixedArray<int, 10> b;
        for (uint16_t i = 0; i < 11; ++i)
            b.push_back(i);
        FAIL();
    }
    catch (...)
    {
    }
}


GTEST_TEST(Utils, FixedArray_003)
{
    FixedArray<int, 10> a;
    a.resize(5);
    for (uint16_t i = 0; i < 5; ++i)
        a[i] = (i);

    EXPECT_EQ(a[0], 0);
    EXPECT_EQ(a[1], 1);
    EXPECT_EQ(a[2], 2);
    EXPECT_EQ(a[3], 3);
    EXPECT_EQ(a[4], 4);

    try
    {
        FixedArray<int, 10> b;
        b.resize(5);
        for (uint16_t i = 0; i < 6; ++i)
            b[i] = i;
        FAIL();
    }
    catch (...)
    {
    }
}


GTEST_TEST(Utils, FixedArray_004)
{
    FixedArray<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    int i = 0;
    for (const auto& elm : a)
    {
        EXPECT_EQ(i, elm);
        ++i;
    }
}

GTEST_TEST(Utils, FixedArray_005)
{
    FixedArray<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(10, a.capacity());
    EXPECT_EQ(10, a.size());
    a.remove(5);
    EXPECT_EQ(10, a.capacity());
    EXPECT_EQ(9, a.size());
    a.remove(6);
    EXPECT_EQ(10, a.capacity());
    EXPECT_EQ(8, a.size());
    a.remove(4);
    EXPECT_EQ(10, a.capacity());
    EXPECT_EQ(7, a.size());

    EXPECT_EQ(a[0], 0);
    EXPECT_EQ(a[1], 1);
    EXPECT_EQ(a[2], 2);
    EXPECT_EQ(a[3], 3);
    EXPECT_EQ(a[4], 6);
    EXPECT_EQ(a[5], 8);
    EXPECT_EQ(a[6], 9);
}

GTEST_TEST(Utils, FixedArray_006)
{
    FixedArray<int, 10> a = {};
    for (uint16_t i = 0; i < 10; ++i)
        a.push_back(i);

    while (!a.empty())
    {
        int exp = a.at(0);
        for (int& i : a)
        {
            EXPECT_EQ(i, exp);
            ++exp;
        }
        a.remove(0);
    }

    for (uint16_t i = 0; i < 10; ++i)
        a.push_back(i);

    while (!a.empty())
    {
        int exp = 0;
        for (int& i : a)
        {
            EXPECT_EQ(i, exp);
            ++exp;
        }
        a.remove(a.size() - 1);
    }
}


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
        Allocator<Complex, size_t, 32> alloc;
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
    using Alloc = Allocator<Complex, size_t>;
    Alloc al;

    Complex* c = al.allocate();
    EXPECT_NE(nullptr, c);
    EXPECT_EQ(123, c->a);
    EXPECT_EQ(456, c->b);

    Alloc::destroy(c);
}

GTEST_TEST(Utils, Array_001)
{
    using IntArray = Array<int, AOP_SIMPLE_TYPE>;
    IntArray ia;

    ia.reserve(20000);
    for (int i = 0; i < 20000; ++i)
        ia.push_back(i);

    const uint32_t pos = ia.findBinary(2);
    EXPECT_EQ(pos, 2);
    EXPECT_EQ(ia[pos], 2);

    while (!ia.empty())
        ia.pop_back();
}

GTEST_TEST(Utils, Array_002)
{
    using IntArray = SimpleArray<int>;
    IntArray ia;

    ia.reserve(20000);

    for (int i = 0; i < 20000; ++i)
        ia.push_back(i);

    EXPECT_EQ(20001, ia.capacity());

    for (uint32_t i = 0; i < 20000; ++i)
        EXPECT_EQ(i, ia[i]);

#if RT_DEBUG == 1
    try
    {
        Console::writeLine((int64_t)ia[ia.capacity()]);
        FAIL();
    }
    catch (...)
    {
    }

    try
    {
        Console::writeLine((int64_t)ia[ia.capacity()]);
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
    using IntArray = SimpleArray<int>;
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
    using IntArray = Array<int, 2, NewAllocator<int, uint32_t, 2>>;
    try
    {
        IntArray ia = {0, 1, 2, 3};
        FAIL();
    }
    catch (...)
    {
        // empty. just care that it throws an exception
        // because to much was allocated
    }
}

GTEST_TEST(Utils, HashTable_001)
{
    using IntTable = HashTable<uint32_t, void*>;
    IntTable it;
    it.reserve(0x08);
    for (uint32_t i = 0; i < 0x10001; ++i)
        it.insert(i, &it);

    for (uint32_t i = 0; i < 0xFFFF; ++i)
        EXPECT_NE(it.find(i), Rt2::Npos32);

    EXPECT_EQ(it.capacity(), 0x20000);
}


GTEST_TEST(Utils, HashTable_002)
{
    using Table = HashTable<String, int>;

    Table table;

    table.insert("aaaaaaaaaaaa1", 1);
    table.insert("aaaaaaaaaaaa2", 2);
    table.insert("aaaaaaaaaaaa3", 3);
    table.insert("aaaaaaa3aaaaa", 4);

    EXPECT_EQ(table.capacity(), 32);
    EXPECT_EQ(table.size(), 4);

    EXPECT_NE(table.find("aaaaaaaaaaaa1"), Npos);
    EXPECT_NE(table.find("aaaaaaaaaaaa2"), Npos);
    EXPECT_NE(table.find("aaaaaaaaaaaa3"), Npos);
    EXPECT_NE(table.find("aaaaaaa3aaaaa"), Npos);

    EXPECT_EQ(table["aaaaaaaaaaaa1"], 1);
    EXPECT_EQ(table["aaaaaaaaaaaa2"], 2);
    EXPECT_EQ(table["aaaaaaaaaaaa3"], 3);
    EXPECT_EQ(table["aaaaaaa3aaaaa"], 4);
}
