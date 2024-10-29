#include "hex_utils.h"
#include <gtest/gtest.h>
#include <pdu.h>

#ifdef __cplusplus
extern "C" {
#endif

static int16_t activePower = 0;
void zigbee_meter_update_active_power(int16_t powerWatts) { activePower = powerWatts; }

static uint16_t rmsCurrent[3] = {};
void zigbee_meter_update_rms_current(int phase, uint16_t currentAmps) {
    EXPECT_TRUE(phase >= 0 && phase <= 2);
    if (phase >= 0 && phase <= 2) {
        rmsCurrent[phase] = currentAmps;
    }
}

static uint16_t rmsVoltage[3] = {};
void zigbee_meter_update_rms_voltage(int phase, uint16_t voltageVolts) {
    EXPECT_TRUE(phase >= 0 && phase <= 2);
    if (phase >= 0 && phase <= 2) {
        rmsVoltage[phase] = voltageVolts;
    }
}

static uint64_t summationReceived = 0;
void zigbee_meter_update_summation_received(uint64_t energy) { summationReceived = energy; }

static uint64_t summationDelivered = 0;
void zigbee_meter_update_summation_delivered(uint64_t energy) { summationDelivered = energy; }

#ifdef __cplusplus
}
#endif

TEST(PduTest, packet) {
    const auto packet = hex2buffer(
        "0F00 4B10AB0C 07E80A1B 0707232DFF800000 020D010D 02041200 28090600 08190900 FF0F0212 00000204 12002809 "
        "06000819 0900FF0F 01120000 02041200 01090600 00600100 FF0F0212 00000204 12000309 06010001 0700FF0F 02120000 "
        "02041200 03090601 00020700 FF0F0212 0000 02 04120003 09060101 010800FF 0F021200 00020412 00030906 01010208 "
        "00FF0F02 12000002 04120003 09060100 1F0700FF 0F021200 00020412 00030906 01003307 00FF0F02 12000002 04120003 "
        "09060100 470700FF 0F021200 00020412 00030906 01002007 00FF0F02 120000 02 04120003 09060100 340700FF 0F021200 "
        "00020412 00030906 01004807 00FF0F02 12000009 06000819 0900FF09 08363636 38313431 30060000 015A0600 00000006 "
        "003D1B04 06000000 00120074 12000E12 00231200 EB1200EA 1200EA");
    ASSERT_TRUE(pdu_decode(&packet));
    EXPECT_EQ(activePower, 346);
    EXPECT_EQ(rmsCurrent[0], 116);
    EXPECT_EQ(rmsCurrent[1], 14);
    EXPECT_EQ(rmsCurrent[2], 35);
    EXPECT_EQ(rmsVoltage[0], 235);
    EXPECT_EQ(rmsVoltage[1], 234);
    EXPECT_EQ(rmsVoltage[2], 234);
    EXPECT_EQ(summationReceived, 4004612);
    EXPECT_EQ(summationDelivered, 0);
}

TEST(PduTest, other) {
    const auto packet =
        hex2buffer("0F004B80 6E0C07E8 0A1C0117 1400FF80 00000206 01060204 12002809 06000919 0900FF0F 02120000 02041200 "
                   "28090600 09190900 FF0F0112 00000204 12000309 06010101 0801FF0F 02120000 02041200 03090601 01010802 "
                   "FF0F0212 00000204 12000309 06010102 0801FF0F 02120000 02041200 03090601 01020802 FF0F0212 00000906 "
                   "00091909 00FF0600 1EDFFC06 001E9F8F 06000000 00060000 0000");
    ASSERT_TRUE(pdu_decode(&packet));
}
