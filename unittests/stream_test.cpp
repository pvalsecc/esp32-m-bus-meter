#include "hex_utils.h"
#include <gtest/gtest.h>
#include <stream.h>

TEST(StreamTest, test) {
    const Buffer buffer = hex2buffer("0102030405060708090A0B0C0D0E");

    Stream stream;
    stream_reset(&stream, &buffer);

    EXPECT_EQ(stream_remains(&stream), 14);

    uint8_t u8;
    ASSERT_TRUE(stream_getu8(&stream, &u8));
    EXPECT_EQ(u8, 0x01);
    EXPECT_EQ(stream_remains(&stream), 13);

    uint16_t u16;
    ASSERT_TRUE(stream_getu16_be(&stream, &u16));
    EXPECT_EQ(u16, 0x0203);
    EXPECT_EQ(stream_remains(&stream), 11);

    ASSERT_TRUE(stream_getu16_le(&stream, &u16));
    EXPECT_EQ(u16, 0x0504);
    EXPECT_EQ(stream_remains(&stream), 9);

    uint32_t u32;
    ASSERT_TRUE(stream_getu32_be(&stream, &u32));
    EXPECT_EQ(u32, 0x06070809);
    EXPECT_EQ(stream_remains(&stream), 5);

    ASSERT_TRUE(stream_skip(&stream, 2));
    EXPECT_EQ(stream_remains(&stream), 3);

    ASSERT_FALSE(stream_getu32_be(&stream, &u32));
    EXPECT_EQ(stream_remains(&stream), 3);

    uint8_t array[2];
    ASSERT_TRUE(stream_get(&stream, array, 2));
    EXPECT_EQ(array[0], 0x0C);
    EXPECT_EQ(array[1], 0x0D);
    EXPECT_EQ(stream_remains(&stream), 1);

    ASSERT_FALSE(stream_get(&stream, array, 2));
    EXPECT_EQ(stream_remains(&stream), 1);

    ASSERT_FALSE(stream_getu16_be(&stream, &u16));
    EXPECT_EQ(stream_remains(&stream), 1);

    ASSERT_FALSE(stream_getu16_le(&stream, &u16));
    EXPECT_EQ(stream_remains(&stream), 1);

    Buffer out;
    buffer_reset(&out);
    ASSERT_TRUE(stream_get_buffer(&stream, &out, 1));
    EXPECT_EQ(out.len, 1);
    EXPECT_EQ(out.bytes[0], 0x0E);
    EXPECT_EQ(stream_remains(&stream), 0);

    ASSERT_FALSE(stream_get_buffer(&stream, &out, 1));
    EXPECT_EQ(stream_remains(&stream), 0);

    ASSERT_FALSE(stream_getu8(&stream, &u8));
    EXPECT_EQ(stream_remains(&stream), 0);
}
