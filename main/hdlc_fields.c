#include "hdlc_fields.h"

bool hdlc_decode_type_length(uint8_t *bytes, int len, int *pos, uint8_t *outType, bool *outSegmentation,
                             uint16_t *outLength) {
    if (len - *pos < 1) {
        return false;
    }
    const uint8_t firstByte = bytes[*pos];
    if ((firstByte & (uint8_t)0x80) == 0) {
        *outType = 0;
        *outLength = firstByte & (uint8_t)0x7F;
        *outSegmentation = false;
        (*bytes)++;
        (*pos)++;
        return true;
    } else {
        if (len - *pos < 2) {
            return false;
        }

        *outType = ((firstByte & (uint8_t)0x70) >> 4) + 1;
        if (*outType >= 1 && *outType <= 7) {
            *outSegmentation = (firstByte & (uint8_t)0x08) != 0;
            *outLength = ((uint16_t)firstByte & (uint16_t)0x07) << 8 | bytes[*pos + 1];
            (*bytes) += 2;
            (*pos) += 2;
            return true;
        } else {
            return false;
        }
    }
}
