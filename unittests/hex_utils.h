#include <buffer.h>
#include <gtest/gtest.h>
#include <inttypes.h>
#include <vector>

static int hex2int(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    } else {
        EXPECT_FALSE(true);
        return 0;
    }
}

static std::vector<uint8_t> hex2vector(const char *hex) {
    std::vector<uint8_t> result;
    while (hex[0] && hex[1]) {
        result.push_back(hex2int(hex[0]) << 4 | hex2int(hex[1]));
        hex += 2;
        while (hex[0] == ' ') {
            ++hex;
        }
    }
    return result;
}

static Buffer hex2buffer(const char *hex) {
    Buffer result;
    buffer_reset(&result);
    while (hex[0] && hex[1]) {
        buffer_add_byte(&result, hex2int(hex[0]) << 4 | hex2int(hex[1]));
        hex += 2;
        while (hex[0] == ' ') {
            ++hex;
        }
    }
    return result;
}
