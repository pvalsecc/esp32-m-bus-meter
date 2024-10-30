#include <buffer.h>
#include <gtest/gtest.h>

TEST(BufferTest, add_byte) {
    Buffer buffer;
    buffer_reset(&buffer);
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        ASSERT_TRUE(buffer_add_byte(&buffer, i));
    }
    ASSERT_FALSE(buffer_add_byte(&buffer, 0));

    EXPECT_EQ(buffer.len, BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        EXPECT_EQ(buffer.bytes[i], i & 0xFF);
    }
}

TEST(BufferTest, add_bytes) {
    Buffer buffer;
    buffer_reset(&buffer);

    uint8_t toAdd[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        toAdd[i] = i;
    }
    ASSERT_TRUE(buffer_add_bytes(&buffer, toAdd, BUFFER_SIZE));
    ASSERT_FALSE(buffer_add_bytes(&buffer, toAdd, 1));

    EXPECT_EQ(buffer.len, BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        EXPECT_EQ(buffer.bytes[i], i & 0xFF);
    }
}
