#include "Utils/Json.h"
#include "ThisDir.h"
#include "Utils/Base64.h"
#include "gtest/gtest.h"

using namespace Rt2::Json;

GTEST_TEST(Json, Token_001)
{
    Internal::Lex::Token tok;
    tok.push_back("\x1F\x41");

    EXPECT_TRUE(tok.value() == "A");
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_UNDEFINED);
    tok.clear();
    EXPECT_TRUE(tok.value().empty());
    EXPECT_EQ(tok.value().size(), 0);
    EXPECT_LE(tok.value().capacity(), 64);

    tok.push_back('H');
    tok.push_back('e');
    tok.push_back('l');
    tok.push_back('l');
    tok.push_back('o');
    tok.push_back(' ');
    tok.push_back('W');
    tok.push_back('o');
    tok.push_back('r');
    tok.push_back('l');
    tok.push_back('d');
    tok.push_back('!');
    EXPECT_TRUE(tok.value() == "Hello World!");
    EXPECT_EQ(tok.value().size(), 12);
    EXPECT_LE(tok.value().capacity(), 64);
}
GTEST_TEST(Json, Scan_001)
{
    Rt2::InputFileStream ifs(TestFile("test2.json"));

    Internal::Lex::Scanner scanner(&ifs);
    EXPECT_TRUE(scanner.isOpen());

    Internal::Lex::Token tok;
    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_L_BRACKET);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_STRING);
    EXPECT_TRUE(tok.value() == "Hello");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COLON);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_STRING);
    EXPECT_TRUE(tok.value() == "World");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COMMA);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_STRING);
    EXPECT_TRUE(tok.value() == "abc");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COLON);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_NUMBER);
    EXPECT_TRUE(tok.value() == "-123.456");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COMMA);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_STRING);
    EXPECT_TRUE(tok.value() == "bool0");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COLON);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_BOOL);
    EXPECT_TRUE(tok.value() == "true");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COMMA);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_STRING);
    EXPECT_TRUE(tok.value() == "bool1");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COLON);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_BOOL);
    EXPECT_TRUE(tok.value() == "false");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COMMA);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_STRING);
    EXPECT_TRUE(tok.value() == "null");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COLON);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_NULL);
    EXPECT_TRUE(tok.value() == "null");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COMMA);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_STRING);
    EXPECT_TRUE(tok.value() == "array");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COLON);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_L_BRACE);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_STRING);
    EXPECT_TRUE(tok.value() == "a");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COMMA);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_INTEGER);
    EXPECT_TRUE(tok.value() == "1");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COMMA);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_BOOL);
    EXPECT_TRUE(tok.value() == "false");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_COMMA);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_NULL);
    EXPECT_TRUE(tok.value() == "null");

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_R_BRACE);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_R_BRACKET);

    scanner.scan(tok);
    EXPECT_EQ(tok.type(), Internal::Lex::Type::JT_EOF);
}

GTEST_TEST(Json, Parse_000)
{
    Document          d0;
    Rt2::StringStream ss0("{}");
    d0.load(ss0);

    EXPECT_TRUE(d0.dictionary().empty());
    EXPECT_TRUE(d0.array().empty());

    Document          d1;
    Rt2::StringStream ss1("[]");
    d1.load(ss1);

    EXPECT_TRUE(d1.dictionary().empty());
    EXPECT_TRUE(d1.array().empty());
}
GTEST_TEST(Json, Parse_001)
{
    Document             doc;
    Rt2::InputFileStream ifs(TestFile("test2.json"));
    doc.load(ifs);

    const Dictionary& dict = doc.dictionary();
    EXPECT_TRUE(dict.contains("Hello"));

    auto v = dict.get("Hello");
    EXPECT_TRUE(v.isString());
    EXPECT_TRUE(v.string() == "World");

    EXPECT_TRUE(dict.contains("abc"));

    v = dict.get("abc");
    EXPECT_TRUE(v.isReal());
    EXPECT_DOUBLE_EQ(v.real(), -123.456);
    EXPECT_EQ(v.integer(), -123);
}

void ValidateTestObject(const Dictionary& dict, bool testArray = false)
{
    EXPECT_TRUE(dict.contains("A"));
    EXPECT_TRUE(dict.contains("B"));
    EXPECT_TRUE(dict.contains("C"));
    EXPECT_TRUE(dict.contains("D"));
    EXPECT_TRUE(dict.contains("W"));
    EXPECT_TRUE(dict.contains("X"));
    EXPECT_TRUE(dict.contains("Y"));
    EXPECT_TRUE(dict.contains("Z"));

    if (testArray)
    {
        EXPECT_TRUE(dict.contains("E"));
    }

    auto v = dict.get("A");
    EXPECT_TRUE(v.isString());
    EXPECT_EQ(v.string(), "String");

    v = dict.get("C");
    EXPECT_TRUE(v.isBoolean());
    EXPECT_EQ(v.boolean(), true);
    EXPECT_EQ(v.integer(), 1);
    EXPECT_DOUBLE_EQ(v.real(), 1.0);
    EXPECT_EQ(v.string(), "1");

    v = dict.get("D");
    EXPECT_TRUE(v.isBoolean());
    EXPECT_EQ(v.boolean(), false);
    EXPECT_EQ(v.integer(), 0);
    EXPECT_DOUBLE_EQ(v.real(), 0.0);
    EXPECT_EQ(v.string(), "0");

    v = dict.get("W");
    EXPECT_TRUE(v.isReal());
    EXPECT_EQ(v.boolean(), true);
    EXPECT_EQ(v.integer(), 3);
    EXPECT_DOUBLE_EQ(v.real(), 3.14);
    EXPECT_EQ(v.string(), "3.14");

    v = dict.get("X");
    EXPECT_TRUE(v.isInteger());
    EXPECT_EQ(v.boolean(), true);
    EXPECT_EQ(v.integer(), 1);
    EXPECT_DOUBLE_EQ(v.real(), 1.0);
    EXPECT_EQ(v.string(), "1");

    v = dict.get("Y");
    EXPECT_TRUE(v.isInteger());
    EXPECT_EQ(v.boolean(), false);
    EXPECT_EQ(v.integer(), 0);
    EXPECT_DOUBLE_EQ(v.real(), 0.0);
    EXPECT_EQ(v.string(), "0");

    v = dict.get("Z");
    EXPECT_TRUE(v.isInteger());
    EXPECT_EQ(v.boolean(), false);
    EXPECT_EQ(v.integer(), 0);
    EXPECT_DOUBLE_EQ(v.real(), 0.0);
    EXPECT_EQ(v.string(), "0");

    if (testArray)
    {
        v = dict.get("E");
        EXPECT_TRUE(v.isArray());

        MixedArray a = v.array();

        EXPECT_EQ(3, a.sizeI());

        for (int i = 0; i < 3; ++i)
        {
            v = a.at(i);
            EXPECT_TRUE(v.isObject());
            ValidateTestObject(v.object(), false);
        }
    }
}

void ValidateTest3(const MixedArray& arr)
{
    EXPECT_EQ(3, arr.size());

    for (int i = 0; i < arr.sizeI(); ++i)
    {
        Value v = arr.at(i);
        EXPECT_TRUE(v.isObject());
        ValidateTestObject(v.object(), true);
    }
}

GTEST_TEST(Json, Parse_002)
{
    Document             doc;
    Rt2::InputFileStream ifs(TestFile("test3.json"));
    doc.load(ifs);

    EXPECT_FALSE(doc.array().empty());
    ValidateTest3(doc.array());
}

GTEST_TEST(Json, Parse_003)
{
    Document             doc;
    Rt2::InputFileStream ifs(TestFile("test4.json"));
    doc.load(ifs);

    EXPECT_TRUE(doc.dictionary().empty());
    EXPECT_FALSE(doc.array().empty());

    const MixedArray& arr = doc.array();
    EXPECT_EQ(7, arr.size());

    EXPECT_EQ(0, arr.at(0).integer());
    EXPECT_EQ(1, arr.at(1).integer());
    EXPECT_EQ(2, arr.at(2).integer());
    EXPECT_EQ(3, arr.at(3).integer());
    EXPECT_EQ(4, arr.at(4).integer());
    EXPECT_EQ(5, arr.at(5).integer());
    EXPECT_EQ("Hello", arr.at(6).string());
}

GTEST_TEST(Json, Parse_004)
{
    Document             d0;
    Rt2::InputFileStream ifs(TestFile("test3.json"));
    d0.load(ifs);
    ValidateTest3(d0.array());

    Document d1;
    d1.load(d0.array().compact());
    ValidateTest3(d1.array());

    Document d2;
    d2.load(d1.array().formatted(20));
    ValidateTest3(d2.array());
}

GTEST_TEST(Json, Parse_005)
{
    Document d0, d1, d3;

    Rt2::String          dest;
    Rt2::InputFileStream ifs(TestFile("test3.json"));
    Rt2::Su::copy(dest, ifs);

    d0.dictionary().insertCode("test3.json", dest);
    d0.dictionary().insert("buffer", Rt2::Base64::encode(dest));

    d1.load(d0.dictionary().compact());
    EXPECT_TRUE(d1.dictionary().contains("test3.json"));

    auto v = d1.dictionary().get("test3.json");
    EXPECT_TRUE(v.isArray());
    ValidateTest3(v.array());

    EXPECT_TRUE(d1.dictionary().contains("buffer"));
    v = d1.dictionary().get("buffer");
    EXPECT_TRUE(v.isString());

    d3.load(Rt2::Base64::decode(v.string()));
    ValidateTest3(d3.array());
}
