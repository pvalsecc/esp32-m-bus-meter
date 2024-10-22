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
