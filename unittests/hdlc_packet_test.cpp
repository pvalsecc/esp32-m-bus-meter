#include "hex_utils.h"
#include <gtest/gtest.h>
#include <hdlc_packet.h>

TEST(Hdlcpacket, type1) {
    std::vector<uint8_t> data = hex2vector(
        "A07D CEFF 03 13 D045 E0400002 00006C02 04120003 09060101 010800FF 0F021200 00020412 00030906 01010208 "
        "00FF0F02 12000002 04120003 09060100 1F0700FF 0F021200 00020412 00030906 01003307 00FF0F02 12000002 "
        "04120003 09060100 470700FF 0F021200 00020412 00030906 01002007 00FF0F02 120000B8 A2");

    Information decoded = hdlc_packet_decode(data.data(), data.size());
    EXPECT_EQ(decoded.size, 0x7D - 8 - 2);
    EXPECT_EQ(decoded.bytes, data.data() + 8);
}

TEST(Hdlcpacket, type2) {
    std::vector<uint8_t> data =
        hex2vector("A06D CEFF 03 13 90F1 E0C00003 00005C02 04120003 09060100 340700FF 0F021200 00020412 00030906 "
                   "01004807 00FF0F02 12000009 06000819 0900FF09 08363636 38313431 30060000 01580600 00000006 003D1B03 "
                   "06000000 00120073 12000E12 00231200 EB1200EB 1200EA2F B0");

    Information decoded = hdlc_packet_decode(data.data(), data.size());
    EXPECT_EQ(decoded.size, 0x6D - 8 - 2);
    EXPECT_EQ(decoded.bytes, data.data() + 8);
}

TEST(Hdlcpacket, type3) {
    std::vector<uint8_t> data = hex2vector(
        "A084 CEFF 03 13 128B E6E700E0 40000100 00700F00 4B10AB0C 07E80A1B 0707232D FF800000 020D010D 02041200 "
        "28090600 08190900 FF0F0212 00000204 12002809 06000819 0900FF0F 01120000 02041200 01090600 00600100 FF0F0212 "
        "00000204 12000309 06010001 0700FF0F 02120000 02041200 03090601 00020700 FF0F0212 000047E0");

    Information decoded = hdlc_packet_decode(data.data(), data.size());
    EXPECT_EQ(decoded.size, 0x84 - 8 - 2);
    EXPECT_EQ(decoded.bytes, data.data() + 8);
}
