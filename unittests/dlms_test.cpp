#include "hex_utils.h"
#include <dlms.h>
#include <gtest/gtest.h>

class DlmsTest : public testing::Test {
  public:
    static void staticHandleDlmsPacket(void *arg, const Buffer *buffer) {
        reinterpret_cast<DlmsTest *>(arg)->handleDlmsPacket(buffer);
    }

    void handleDlmsPacket(const Buffer *buffer) {
        _packets.push_back(std::vector<uint8_t>(buffer->bytes, buffer->bytes + buffer->len));
    }

    std::vector<std::vector<uint8_t>> _packets;
};

TEST_F(DlmsTest, sample) {
    const auto packet1 =
        hex2buffer("E6E700E0 40 0001 0000 70 0F00 4B10AB0C 07E80A1B 0707232D FF800000 020D010D 02041200 28090600 "
                   "08190900 FF0F0212 00000204 12002809 06000819 0900FF0F 01120000 02041200 01090600 00600100 FF0F0212 "
                   "00000204 12000309 06010001 0700FF0F 02120000 02041200 03090601 00020700 FF0F0212 0000");

    const auto packet2 =
        hex2buffer("E0       40 0002 0000 6C 02 04120003 09060101 010800FF 0F021200 00020412 00030906 01010208 "
                   "00FF0F02 12000002 04120003 09060100 1F0700FF 0F021200 00020412 00030906 01003307 00FF0F02 12000002 "
                   "04120003 09060100 470700FF 0F021200 00020412 00030906 01002007 00FF0F02 120000");

    const auto packet3 =
        hex2buffer("E0       C0 0003 0000 5C 02 04120003 09060100 340700FF 0F021200 00020412 00030906 01004807 "
                   "00FF0F02 12000009 06000819 0900FF09 08363636 38313431 30060000 015A0600 00000006 003D1B04 06000000 "
                   "00120074 12000E12 00231200 EB1200EA 1200EA");

    dlms_state *state = dlms_init(&staticHandleDlmsPacket, this);
    ASSERT_TRUE(dlms_decode(state, &packet1));
    EXPECT_EQ(_packets.size(), 0);
    ASSERT_TRUE(dlms_decode(state, &packet2));
    EXPECT_EQ(_packets.size(), 0);
    ASSERT_TRUE(dlms_decode(state, &packet3));
    EXPECT_EQ(_packets.size(), 1);

    EXPECT_EQ(_packets, std::vector<std::vector<uint8_t>>({hex2vector(
                            // from the first packet
                            "0F00 4B10AB0C 07E80A1B 0707232D FF800000 020D010D 02041200 28090600 08190900 FF0F0212 "
                            "00000204 12002809 06000819 0900FF0F 01120000 02041200 01090600 00600100 FF0F0212 00000204 "
                            "12000309 06010001 0700FF0F 02120000 02041200 03090601 00020700 FF0F0212 0000"

                            // from the second packet
                            "02 04120003 09060101 010800FF 0F021200 00020412 00030906 01010208 00FF0F02 12000002 "
                            "04120003 09060100 1F0700FF 0F021200 00020412 00030906 01003307 00FF0F02 12000002 04120003 "
                            "09060100 470700FF 0F021200 00020412 00030906 01002007 00FF0F02 120000"

                            // from the third packet
                            "02 04120003 09060100 340700FF 0F021200 00020412 00030906 01004807 00FF0F02 12000009 "
                            "06000819 0900FF09 08363636 38313431 30060000 015A0600 00000006 003D1B04 06000000 00120074 "
                            "12000E12 00231200 EB1200EA 1200EA")}));
}
