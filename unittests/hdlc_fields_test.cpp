#include "hex_utils.h"
#include <gtest/gtest.h>
#include <hdlc_fields.h>
#include <stream.h>

TEST(HdlcFields, type0) {
    const auto buffer = hex2buffer("42");
    Stream stream;
    stream_reset(&stream, &buffer);
    uint8_t outType;
    bool outSegmentation = true;
    uint16_t outLen;
    ASSERT_TRUE(hdlc_decode_type_length(&stream, &outType, &outSegmentation, &outLen));
    EXPECT_EQ(outType, 0);
    EXPECT_FALSE(outSegmentation);
    EXPECT_EQ(outLen, 0x42);
    EXPECT_EQ(stream_remains(&stream), 0);
}

TEST(HdlcFields, type1) {
    const auto buffer = hex2buffer("8101");
    Stream stream;
    stream_reset(&stream, &buffer);
    uint8_t outType;
    bool outSegmentation = true;
    uint16_t outLen;
    ASSERT_TRUE(hdlc_decode_type_length(&stream, &outType, &outSegmentation, &outLen));
    EXPECT_EQ(outType, 1);
    EXPECT_FALSE(outSegmentation);
    EXPECT_EQ(outLen, 0x101);
    EXPECT_EQ(stream_remains(&stream), 0);
}

TEST(HdlcFields, type1WithSegmentation) {
    const auto buffer = hex2buffer("8805");
    Stream stream;
    stream_reset(&stream, &buffer);
    uint8_t outType;
    bool outSegmentation = false;
    uint16_t outLen;
    ASSERT_TRUE(hdlc_decode_type_length(&stream, &outType, &outSegmentation, &outLen));
    EXPECT_EQ(outType, 1);
    EXPECT_TRUE(outSegmentation);
    EXPECT_EQ(outLen, 0x005);
    EXPECT_EQ(stream_remains(&stream), 0);
}

TEST(HdlcFields, type3) {
    const auto buffer = hex2buffer("A321");
    Stream stream;
    stream_reset(&stream, &buffer);
    uint8_t outType;
    bool outSegmentation = true;
    uint16_t outLen;
    ASSERT_TRUE(hdlc_decode_type_length(&stream, &outType, &outSegmentation, &outLen));
    EXPECT_EQ(outType, 3);
    EXPECT_FALSE(outSegmentation);
    EXPECT_EQ(outLen, 0x321);
    EXPECT_EQ(stream_remains(&stream), 0);
}

TEST(HdlcFields, address) {
    const auto buffer = hex2buffer("CEFF031390F1E0");
    Stream stream;
    stream_reset(&stream, &buffer);
    int outAddressLen = 0;

    // decode first address
    ASSERT_TRUE(hdlc_decode_address(&stream, &outAddressLen));
    EXPECT_EQ(outAddressLen, 2);
    EXPECT_EQ(stream.pos, 2);

    // decode second address
    ASSERT_TRUE(hdlc_decode_address(&stream, &outAddressLen));
    EXPECT_EQ(outAddressLen, 1);
    EXPECT_EQ(stream.pos, 3);
}

TEST(HdlcFields, control1) {
    const auto buffer = hex2buffer("1390F1E0");
    Stream stream;
    stream_reset(&stream, &buffer);
    ControlField controlField = {};

    ASSERT_TRUE(hdlc_decode_control(&stream, &controlField));
    EXPECT_EQ(stream.pos, 1);
    EXPECT_EQ(controlField.type, UNNUMBERED_COMMAND);
    EXPECT_EQ(controlField.finalBit, true);
}

TEST(HdlcFields, crc16a) {
    const auto buffer = hex2buffer("A06DCEFF031390F1");
    Stream stream;
    stream_reset(&stream, &buffer);
    stream_skip(&stream, 6);

    ASSERT_TRUE(hdlc_decode_crc16(&stream));
}

TEST(HdlcFields, crc16b) {
    const auto buffer = hex2buffer("A084CEFF0313128B}");
    Stream stream;
    stream_reset(&stream, &buffer);
    stream_skip(&stream, 6);
    ASSERT_TRUE(hdlc_decode_crc16(&stream));
}

TEST(HdlcFields, crc16c) {
    const auto buffer = hex2buffer("A07DCEFF0313D045}");
    Stream stream;
    stream_reset(&stream, &buffer);
    stream_skip(&stream, 6);
    ASSERT_TRUE(hdlc_decode_crc16(&stream));
}
