#include <gtest/gtest.h>
#include <hdlc_fields.h>

TEST(HdlcFields, type0) {
    uint8_t bytes[] = {0x42};
    int pos = 0;
    uint8_t outType;
    bool outSegmentation = true;
    uint16_t outLen;
    ASSERT_TRUE(hdlc_decode_type_length(bytes, sizeof(bytes), &pos, &outType, &outSegmentation, &outLen));
    EXPECT_EQ(outType, 0);
    EXPECT_FALSE(outSegmentation);
    EXPECT_EQ(outLen, 0x42);
    EXPECT_EQ(pos, sizeof(bytes));
}

TEST(HdlcFields, type1) {
    uint8_t bytes[] = {0x81, 0x01};
    int pos = 0;
    uint8_t outType;
    bool outSegmentation = true;
    uint16_t outLen;
    ASSERT_TRUE(hdlc_decode_type_length(bytes, sizeof(bytes), &pos, &outType, &outSegmentation, &outLen));
    EXPECT_EQ(outType, 1);
    EXPECT_FALSE(outSegmentation);
    EXPECT_EQ(outLen, 0x101);
    EXPECT_EQ(pos, sizeof(bytes));
}

TEST(HdlcFields, type1WithSegmentation) {
    uint8_t bytes[] = {0x88, 0x05};
    int pos = 0;
    uint8_t outType;
    bool outSegmentation = false;
    uint16_t outLen;
    ASSERT_TRUE(hdlc_decode_type_length(bytes, sizeof(bytes), &pos, &outType, &outSegmentation, &outLen));
    EXPECT_EQ(outType, 1);
    EXPECT_TRUE(outSegmentation);
    EXPECT_EQ(outLen, 0x005);
    EXPECT_EQ(pos, sizeof(bytes));
}

TEST(HdlcFields, type3) {
    uint8_t bytes[] = {0xA3, 0x21};
    int pos = 0;
    uint8_t outType;
    bool outSegmentation = true;
    uint16_t outLen;
    ASSERT_TRUE(hdlc_decode_type_length(bytes, sizeof(bytes), &pos, &outType, &outSegmentation, &outLen));
    EXPECT_EQ(outType, 3);
    EXPECT_FALSE(outSegmentation);
    EXPECT_EQ(outLen, 0x321);
    EXPECT_EQ(pos, sizeof(bytes));
}

TEST(HdlcFields, address) {
    uint8_t bytes[] = {0xCE, 0xFF, 0x03, 0x13, 0x90, 0xF1, 0xE0};
    int pos = 0;
    int outAddressLen = 0;

    // decode first address
    ASSERT_TRUE(hdlc_decode_address(bytes, sizeof(bytes), &pos, &outAddressLen));
    EXPECT_EQ(outAddressLen, 2);
    EXPECT_EQ(pos, 2);

    // decode second address
    ASSERT_TRUE(hdlc_decode_address(bytes, sizeof(bytes), &pos, &outAddressLen));
    EXPECT_EQ(outAddressLen, 1);
    EXPECT_EQ(pos, 3);
}

TEST(HdlcFields, control1) {
    uint8_t bytes[] = {0x13, 0x90, 0xF1, 0xE0};
    int pos = 0;
    ControlField controlField = {};

    ASSERT_TRUE(hdlc_decode_control(bytes, sizeof(bytes), &pos, &controlField));
    EXPECT_EQ(pos, 1);
    EXPECT_EQ(controlField.type, UNNUMBERED_COMMAND);
    EXPECT_EQ(controlField.finalBit, true);
}

TEST(HdlcFields, crc16a) {
    uint8_t bytes[] = {0xA0, 0x6D, 0xCE, 0xFF, 0x03, 0x13, 0x90, 0xF1};
    int pos = sizeof(bytes) - 2;
    ASSERT_TRUE(hdlc_decode_crc16(bytes, sizeof(bytes), &pos));
}

TEST(HdlcFields, crc16b) {
    uint8_t bytes[] = {0xA0, 0x84, 0xCE, 0xFF, 0x03, 0x13, 0x12, 0x8B};
    int pos = sizeof(bytes) - 2;
    ASSERT_TRUE(hdlc_decode_crc16(bytes, sizeof(bytes), &pos));
}

TEST(HdlcFields, crc16c) {
    uint8_t bytes[] = {0xA0, 0x7D, 0xCE, 0xFF, 0x03, 0x13, 0xD0, 0x45};
    int pos = sizeof(bytes) - 2;
    ASSERT_TRUE(hdlc_decode_crc16(bytes, sizeof(bytes), &pos));
}
