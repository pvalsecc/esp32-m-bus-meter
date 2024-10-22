#include <gtest/gtest.h>
#include <hdlc_frame.h>

class HdlcFrameTest : public testing::Test {
  public:
    void cb(uint8_t *buffer, int len) {
        std::vector<uint8_t> data(buffer, buffer + len);
        frames.push_back(data);
    }

    std::vector<std::vector<uint8_t>> frames;
};

void staticCb(void *arg, uint8_t *buffer, int len) {
    auto self = reinterpret_cast<HdlcFrameTest *>(arg);
    self->cb(buffer, len);
}

TEST_F(HdlcFrameTest, simpleFrame) {
    auto state = hdlc_frame_init(&staticCb, this);
    hdlc_handle_byte(state, 0x7E);
    hdlc_handle_byte(state, 0x01);
    hdlc_handle_byte(state, 0x02);
    hdlc_handle_byte(state, 0x7E);

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({std::vector<uint8_t>({0x01, 0x02})}));
}

TEST_F(HdlcFrameTest, twoFramesWithSingleStartByte) {
    auto state = hdlc_frame_init(&staticCb, this);
    hdlc_handle_byte(state, 0x7E);
    hdlc_handle_byte(state, 0x01);
    hdlc_handle_byte(state, 0x02);
    hdlc_handle_byte(state, 0x7E);
    hdlc_handle_byte(state, 0x03);
    hdlc_handle_byte(state, 0x04);
    hdlc_handle_byte(state, 0x05);
    hdlc_handle_byte(state, 0x7E);

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({
                          std::vector<uint8_t>({0x01, 0x02}),
                          std::vector<uint8_t>({0x03, 0x04, 0x05}),
                      }));
}

TEST_F(HdlcFrameTest, twoFramesWithDoubleStartByte) {
    auto state = hdlc_frame_init(&staticCb, this);
    hdlc_handle_byte(state, 0x7E);
    hdlc_handle_byte(state, 0x01);
    hdlc_handle_byte(state, 0x02);
    hdlc_handle_byte(state, 0x7E);
    hdlc_handle_byte(state, 0x7E);
    hdlc_handle_byte(state, 0x03);
    hdlc_handle_byte(state, 0x04);
    hdlc_handle_byte(state, 0x05);
    hdlc_handle_byte(state, 0x7E);

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({
                          std::vector<uint8_t>({0x01, 0x02}),
                          std::vector<uint8_t>({0x03, 0x04, 0x05}),
                      }));
}

TEST_F(HdlcFrameTest, frameWithEscapeSequence) {
    auto state = hdlc_frame_init(&staticCb, this);
    hdlc_handle_byte(state, 0x7E);
    hdlc_handle_byte(state, 0x01);
    hdlc_handle_byte(state, 0x7D);
    hdlc_handle_byte(state, 0x5E);
    hdlc_handle_byte(state, 0x02);
    hdlc_handle_byte(state, 0x7E);

    EXPECT_EQ(frames, std::vector<std::vector<uint8_t>>({std::vector<uint8_t>({0x01, 0x7E, 0x02})}));
}
