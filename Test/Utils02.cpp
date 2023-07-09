#include "Utils/Allocator.h"
#include "Utils/Array.h"
#include "Utils/Char.h"
#include "Utils/Directory/Path.h"
#include "Utils/HashMap.h"
#include "Utils/ListBinaryTree.h"
#include "Utils/ObjectPool.h"
#include "Utils/SymbolStream.h"
#include "gtest/gtest.h"

using namespace Rt2;

GTEST_TEST(Utils, Console_001)
{
    constexpr int index[] = {
        CS_BLACK,
        CS_DARK_BLUE,
        CS_DARK_GREEN,
        CS_DARK_CYAN,
        CS_DARK_RED,
        CS_DARK_MAGENTA,
        CS_DARK_YELLOW,
        CS_LIGHT_GREY,
        CS_GREY,
        CS_BLUE,
        CS_GREEN,
        CS_CYAN,
        CS_RED,
        CS_MAGENTA,
        CS_YELLOW,
        CS_WHITE,
    };
    Con::resetColor();
    for (auto i : index)
    {
        Con::setForeground((ConsoleColor)i);
        Con::print("#");
    }
    Con::nl();
    for (auto i : index)
    {
        Con::setBackground((ConsoleColor)i);
        Con::print("#");
    }
    Con::nl();
}

GTEST_TEST(Utils, SymbolStream_001)
{
    SymbolStream ss;
    ss.setBase(2);
    String s;
    ss.base(s, 0b101000000011111);
    EXPECT_EQ(s, "101000000011111");

    ss.setBase(16);
    ss.base(s, 0xFF764AC0);
    EXPECT_EQ(s, "FF764AC0");

    ss.setBase(8);
    ss.base(s, 511);
    EXPECT_EQ(s, "777");
}

GTEST_TEST(Utils, SymbolStream_002)
{
    {
        SymbolStream ss({'A', 'B'});
        ss.setBase(2);
        String s;
        ss.base(s, 0b101000000011111);
        EXPECT_EQ(s, "BABAAAAAAABBBBB");
    }

    {
        SymbolStream ss({'F', 'E', 'D', 'C', 'B', 'A', '9', '8', '7', '6', '5', '4', '3', '2', '1', '0'});
        String       s;
        ss.setBase(16);
        ss.base(s, 0x89C53F0);
        EXPECT_EQ(s, "763AC0F");
    }

    {
        SymbolStream ss({'H', 'E', 'l', 'o', '[', '/', '-', '!', '$', '^', '5', '4', '%', '2', '1', '0'});
        String       s;
        ss.setPad(true);
        ss.setBase((int)ss.size());
        ss.base(s, "Hello World!");
        EXPECT_EQ(s, "[$-/-%-%-0lH/!-0!l-%-[lE");
    }
}

constexpr int List[] = {50, 30, 70, 20, 40, 60, 80, 32, 34, 36, 65, 75, 85};
constexpr int C1[]   = {36, 20, 65, 75, 85};
constexpr int C2[]   = {32, 60, 40};
constexpr int C3[]   = {30, 70, 50};
using SearchTree     = PairedBinaryTree<int, int, 0x40>;

void populate(SearchTree& tree, const int initial[], const size_t len)
{
    for (size_t i = 0; i < len; ++i)
        tree.insert(initial[i], initial[i]);

    // for (const auto it : tree)
    //    Rt2::Console::print(it, ' ');
    // Console::nl();
}

void remove(SearchTree& tree,
            const int   removeList[],
            const int   removeListLen)
{
    for (int i = 0; i < removeListLen; ++i)
    {
        if (int v = removeList[i]; tree.contains(v))
        {
            // Console::print("Removing ", v, ": = ", ' ');

            tree.erase(v);
            // for (const auto it : tree)
            //    Console::print(it, ' ');
            // Console::nl();
        }
    }
    // Console::nl();
}

GTEST_TEST(Utils, ListBinaryTree_001)
{
    // should produce the following output

    // 20 30 32 34 36 40 50 60 65 70 75 80 85
    // Removing 36: = 20 30 32 34 40 50 60 65 70 75 80 85
    // Removing 20: = 30 32 34 40 50 60 65 70 75 80 85
    // Removing 65: = 30 32 34 40 50 60 70 75 80 85
    // Removing 75: = 30 32 34 40 50 60 70 80 85
    // Removing 85: = 30 32 34 40 50 60 70 80
    //
    // Removing 50: = 80 70 60 40 34 32 30
    // Removing 30: = 80 70 60 40 34 32
    // Removing 70: = 80 60 40 34 32
    // Removing 40: = 80 60 34 32
    // Removing 60: = 80 34 32
    // Removing 80: = 34 32
    // Removing 32: = 34
    // Removing 34: =
    SearchTree         tree;
    SearchTree::Values it;

    constexpr int len   = sizeof List / sizeof(int);
    constexpr int c1Len = sizeof C1 / sizeof(int);

    populate(tree, List, len);
    remove(tree, C1, c1Len);

    constexpr int result[] = {30, 32, 34, 40, 50, 60, 70, 80};

    int r = 0;

    for (const auto v : tree)
    {
        if (r < len)
            EXPECT_EQ(result[r], v);
        ++r;
    }

    remove(tree, List, len);

    EXPECT_EQ(true, tree.left() == nullptr);
    EXPECT_EQ(true, tree.right() == nullptr);
}

GTEST_TEST(Utils, ListBinaryTree_002)
{
    // should produce the following output
    // 20 30 32 34 36 40 50 60 65 70 75 80 85
    // Removing 32: = 20 30 34 36 40 50 60 65 70 75 80 85
    // Removing 60: = 20 30 34 36 40 50 65 70 75 80 85
    // Removing 40: = 20 30 34 36 50 65 70 75 80 85
    //
    // Removing 50: = 85 80 75 70 65 36 34 30 20
    // Removing 30: = 85 80 75 70 65 36 34 20
    // Removing 70: = 85 80 75 65 36 34 20
    // Removing 20: = 85 80 75 65 36 34
    // Removing 80: = 85 75 65 36 34
    // Removing 34: = 85 75 65 36
    // Removing 36: = 85 75 65
    // Removing 65: = 85 75
    // Removing 75: = 85
    // Removing 85: =

    SearchTree tree;

    constexpr size_t len   = std::size(List);
    constexpr size_t c2Len = std::size(C2);

    populate(tree, List, len);
    remove(tree, C2, c2Len);

    const int result[] = {20, 30, 34, 36, 50, 65, 70, 75, 80, 85};

    int r = 0;

    for (const auto v : tree)
    {
        if (r < len)
            EXPECT_EQ(result[r], v);
        ++r;
    }

    remove(tree, List, len);
    EXPECT_EQ(true, tree.left() == nullptr);
    EXPECT_EQ(true, tree.right() == nullptr);
}

GTEST_TEST(Utils, ListBinaryTree_003)
{
    // should produce the following output
    // 20 30 32 34 36 40 50 60 65 70 75 80 85
    // Removing 30: = 20 32 34 36 40 50 60 65 70 75 80 85
    // Removing 70: = 20 32 34 36 40 50 60 65 75 80 85
    // Removing 50: = 20 32 34 36 40 60 65 75 80 85
    //
    // Removing 20: = 85 80 75 65 60 40 36 34 32
    // Removing 40: = 85 80 75 65 60 36 34 32
    // Removing 60: = 85 80 75 65 36 34 32
    // Removing 80: = 85 75 65 36 34 32
    // Removing 32: = 85 75 65 36 34
    // Removing 34: = 85 75 65 36
    // Removing 36: = 85 75 65
    // Removing 65: = 85 75
    // Removing 75: = 85
    // Removing 85: =

    SearchTree         tree;
    SearchTree::Values it;

    constexpr int len   = sizeof(List) / sizeof(int);
    constexpr int c3Len = sizeof(C3) / sizeof(int);

    populate(tree, List, len);
    remove(tree, C3, c3Len);

    constexpr int arr[] = {20, 32, 34, 36, 40, 60, 65, 75, 80, 85};

    int r = 0;

    for (const auto v : tree)
    {
        if (r < len)
            EXPECT_EQ(arr[r], v);
        ++r;
    }

    remove(tree, List, len);

    EXPECT_EQ(true, tree.left() == nullptr);
    EXPECT_EQ(true, tree.right() == nullptr);
}

GTEST_TEST(Utils, ListBinaryTree_004)
{
    SearchTree tree;

    int len = sizeof(List) / sizeof(int);
    populate(tree, List, len);

    SearchTree::Node* node = tree.root();
    SearchTree::Node* l;
    SearchTree::Node* r;
    EXPECT_EQ(true, node != nullptr);
    EXPECT_EQ(50, node->key());

    node = node->left();
    EXPECT_EQ(true, node != nullptr);
    EXPECT_EQ(30, node->key());

    l = node->left();
    EXPECT_EQ(true, l != nullptr);
    EXPECT_EQ(20, l->key());

    r = node->right();
    EXPECT_EQ(true, r != nullptr);
    EXPECT_EQ(40, r->key());

    EXPECT_EQ(true, r->right() == nullptr);

    l = r->left();
    EXPECT_EQ(true, l != nullptr);
    EXPECT_EQ(32, l->key());

    EXPECT_EQ(true, l->left() == nullptr);

    r = tree.maximum(l->right());
    EXPECT_EQ(true, r != nullptr);
    EXPECT_EQ(36, r->key());

    node = tree.root();
    r    = node->right();
    EXPECT_EQ(true, r != nullptr);
    EXPECT_EQ(70, r->key());

    l = tree.maximum(r->left());
    EXPECT_EQ(true, l != nullptr);
    EXPECT_EQ(65, l->key());

    r = tree.maximum(r->right());
    EXPECT_EQ(85, r->key());
}

GTEST_TEST(Utils, ListBinaryTree_005)
{
    using StringTree = PairedBinaryTree<hash_t, String>;

    const hash_t k0 = Hash("aaa");
    const hash_t k1 = Hash("bbb");
    const hash_t k2 = Hash("ccc");

    StringTree tree;
    tree.insert(k0, "aaa");
    tree.insert(k1, "bbb");
    tree.insert(k2, "ccc");

    EXPECT_TRUE(tree.contains(k0));
    EXPECT_TRUE(tree.contains(k1));
    EXPECT_TRUE(tree.contains(k2));

    EXPECT_TRUE(tree.get(k0) == "aaa");
    EXPECT_TRUE(tree.get(k1) == "bbb");
    EXPECT_TRUE(tree.get(k2) == "ccc");
}

GTEST_TEST(Utils, ListBinaryTree_006)
{
    using StringTree = PairedBinaryTree<hash_t, String>;
    hash_t keys[100];
    String values[100];

    StringTree tree;

    for (int i = 0; i < 100; i++)
    {
        String val = Su::join("Foo Bar;", i);
        values[i]  = val;
        keys[i]    = Hash(val);
        tree.insert(keys[i], val);
    }

    for (int i = 0; i < 100; i++)
    {
        String expected = values[i];
        EXPECT_TRUE(tree.contains(keys[i]));
        EXPECT_EQ(tree.get(keys[i]), expected);
    }

    for (int i = 0; i < 100; i++)
    {
        if (i % 2 == 0)
            tree.erase(keys[i]);
    }

    EXPECT_EQ(tree.size(), 50);

    for (int i = 0; i < 100; i++)
    {
        if (i % 2 == 0)
        {
            EXPECT_FALSE(tree.contains(keys[i]));
        }
        else
        {
            EXPECT_TRUE(tree.contains(keys[i]));
        }
    }

    StringTree::Keys ke;
    tree.keys(ke);

    hash_t prev = 0;
    for (const auto& k : ke)
    {
        EXPECT_TRUE(k > prev);
        prev = k;
    }

    StringTree::Values vl;
    tree.values(vl);

    Array<int> counts;

    for (const auto& v : vl)
    {
        StringArray d;
        Su::split(d, v, ';');
        EXPECT_EQ(d.size(), 2);
        int iv = Char::toInt32(d[1]);
        EXPECT_TRUE(iv % 2 != 0);
        counts.push_back(iv);
    }
    std::sort(counts.begin(), counts.end());

    int n = 0;
    for (int i = 1; i < 100; i += 2, ++n)
    {
        EXPECT_TRUE(n < counts.sizeI());
        EXPECT_EQ(counts[n], i);
        // Console::println(counts[n]);
    }
}

GTEST_TEST(Utils, ObjectPool_001)
{
    constexpr int m1 = 0x80;
    constexpr int m2 = m1 * m1;

    using RectPool = ObjectPool<int, m1>;

    RectPool a, b;
    for (int i = 0; i < m2; ++i)
    {
        int* r = a.allocate();

        // tests the new placement call in free.
        // Reset any previous state of it with
        // the default constructor
        *r = m2;
        a.free(r);
        r = a.allocate();
        EXPECT_NE(*r, m2);

        *r = m2;
        b.free(r);
    }

    EXPECT_EQ(a.capacity(), m1);
    EXPECT_EQ(b.capacity(), m2 + m1);
}
