#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Stream;

bool hdlc_decode_type_length(struct Stream *stream, uint8_t *outType, bool *outSegmentation, uint16_t *outLength);

bool hdlc_decode_address(struct Stream *stream, int *outAddressLen);

typedef enum { INFORMATION_TRANSFER_COMMAND, SUPERVISORY_COMMAND, UNNUMBERED_COMMAND } ControlType;

typedef struct {
    uint8_t sendSequenceNumber;
    uint8_t receiveSequenceNumber;
} InformationTransferCommand;

typedef struct {
    bool finalBit;
} UnnumberedCommand;

typedef struct {
    ControlType type;
    bool finalBit;
    union {
        InformationTransferCommand informationTransferCommand;
        UnnumberedCommand unnumberedCommand;
    };
} ControlField;

bool hdlc_decode_control(struct Stream *stream, ControlField *controlfield);

bool hdlc_decode_crc16(struct Stream *stream);

#ifdef __cplusplus
}
#endif
