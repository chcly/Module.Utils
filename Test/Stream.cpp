#include "Utils/Base64.h"
#include "Utils/Json.h"
#include "Utils/StreamConverters/Hex.h"
#include "Utils/Streams/StreamBase.h"
#include "Utils/String.h"
#include "gtest/gtest.h"

using namespace Rt2::Json;

GTEST_TEST(Stream, Stream_001)
{
    Rt2::OutputBufferStream os;
    os.print(Rt2::Base64::encode("foobar"));
    os.print(Rt2::Hex((uint8_t)123));
    os.print(nullptr);
    Rt2::String s1;
    os.string(s1);
    EXPECT_EQ(s1, "Zm9vYmFy7Bnullptr");

    Rt2::InputStringStream iss(s1);

    Rt2::OutputBufferStream b1;
    b1.copy(iss);

    Rt2::String s2;
    b1.string(s2);
    EXPECT_EQ(s1, s2);
    b1.reset();
    Rt2::String s3;
    b1.string(s3);
    EXPECT_EQ(s3, "");
    b1.print(1);
    b1.string(s3);
    EXPECT_EQ(s3, "1");
}

GTEST_TEST(Stream, Stream_002)
{
    Rt2::OutputBufferStream os;
    os.print(123, ',', 456, ',', 789, ',', 10, ',', 11, ',', 12);
    EXPECT_EQ(os.string(), "123,456,789,10,11,12");


    Rt2::InputBufferStream is(os.string());

    char c;
    int  v0, v1, v2, v3, v4, v5;
    is.get(v0, c, v1, c, v2, c, v3, c, v4, c, v5);

    EXPECT_EQ(v0, 123);
    EXPECT_EQ(v1, 456);
    EXPECT_EQ(v2, 789);
    EXPECT_EQ(v3, 10);
    EXPECT_EQ(v4, 11);
    EXPECT_EQ(v5, 12);
}
