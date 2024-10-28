#include "hex_utils.h"
#include <gtest/gtest.h>
#include <hdlc_frame.h>

class HdlcFrameTest : public testing::Test {
  public:
    void SetUp() override { state = hdlc_frame_init(&staticCb, this); }

  public:
    static void staticCb(void *arg, const uint8_t *buffer, int len) {
        auto self = reinterpret_cast<HdlcFrameTest *>(arg);
        self->cb(buffer, len);
    }

    void cb(const uint8_t *buffer, int len) {
        const std::vector<uint8_t> data(buffer, buffer + len);
        frames.push_back(data);
    }

    void hdlc_handle_bytes(const char *hex) const {
        for (uint8_t byte : hex2vector(hex)) {
            hdlc_handle_byte(state, byte);
            hex += 2;
        }
    }

    std::vector<std::vector<uint8_t>> frames;
    _hdlc_frame_state *state;
};

TEST_F(HdlcFrameTest, simpleFrame) {
    hdlc_handle_bytes("7EA003037E");

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({std::vector<uint8_t>({0xA0, 0x03, 0x03})}));
}

TEST_F(HdlcFrameTest, twoFramesWithSingleStartByte) {
    hdlc_handle_bytes("7EA003047E");
    hdlc_handle_bytes("A00405067E");

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({
                          std::vector<uint8_t>({0xA0, 0x03, 0x04}),
                          std::vector<uint8_t>({0xA0, 0x04, 0x05, 0x06}),
                      }));
}

TEST_F(HdlcFrameTest, twoFramesWithDoubleStartByte) {
    hdlc_handle_bytes("7EA003047E");
    hdlc_handle_bytes("7EA00405067E");

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({
                          std::vector<uint8_t>({0xA0, 0x03, 0x04}),
                          std::vector<uint8_t>({0xA0, 0x04, 0x05, 0x06}),
                      }));
}
#ifdef HANDLE_ESCAPE

TEST_F(HdlcFrameTest, frameWithEscapeSequence) {
    hdlc_handle_bytes("7EA0047D5E027E");

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({std::vector<uint8_t>({0xA0, 0x04, 0x7E, 0x02})}));
}
#endif

TEST_F(HdlcFrameTest, frameTooBig) {
    hdlc_handle_bytes("7EA1F6");
    for (int i = 0; i < 500; ++i) {
        hdlc_handle_byte(state, 0x01);
    }
    hdlc_handle_byte(state, 0x7E);
    hdlc_handle_bytes("A003027E");

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({std::vector<uint8_t>({0xA0, 0x03, 0x02})}));
}

TEST_F(HdlcFrameTest, ignoreStuffAtTheBegining) {
    hdlc_handle_bytes("424344");
    hdlc_handle_bytes("7EA003027E");

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({std::vector<uint8_t>({0xA0, 0x03, 0x02})}));
}

TEST_F(HdlcFrameTest, landisGyrIsStupid) {
    hdlc_handle_bytes("7EA0047DFF7E");
    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({std::vector<uint8_t>({0xA0, 0x04, 0x7D, 0xFF})}));
}
