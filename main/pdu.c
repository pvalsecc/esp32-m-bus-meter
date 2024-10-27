#include "pdu.h"
#include <esp_log.h>

// 0F00 4B10AB0C 07E80A1B 0707232DFF800000 020D010D 02041200 28090600 08190900 FF0F0212 00000204 12002809 06000819
// 0900FF0F 01120000 02041200 01090600 00600100 FF0F0212 00000204 12000309 06010001 0700FF0F 02120000 02041200 03090601
// 00020700 FF0F0212 0000 02 04120003 09060101 010800FF 0F021200 00020412 00030906 01010208 00FF0F02 12000002 04120003
// 09060100 1F0700FF 0F021200 00020412 00030906 01003307 00FF0F02 12000002 04120003 09060100 470700FF 0F021200 00020412
// 00030906 01002007 00FF0F02 120000 02 04120003 09060100 340700FF 0F021200 00020412 00030906 01004807 00FF0F02 12000009
// 06000819 0900FF09 08363636 38313431 30060000 015A0600 00000006 003D1B04 06000000 00120074 12000E12 00231200 EB1200EA
// 1200EA

// <DataNotification>
//  <LongInvokeIdAndPriority Value="004B10AB" />
//  <!--2024-10-27 07:35:45-->
//  <DateTime Value="07E80A1B0707232DFF800000" />
//  <NotificationBody>
//    <DataValue>
//      <Structure Qty="0D" >
//        <Array Qty="0D" >
//          <Structure Qty="04" >
//            <UInt16 Value="0028" />
//            <!--0.8.25.9.0.255-->
//            <OctetString Value="0008190900FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0028" />
//            <!--0.8.25.9.0.255-->
//            <OctetString Value="0008190900FF" />
//            <Int8 Value="01" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0001" />
//            <!--0.0.96.1.0.255 N° série fabricant -->
//            <OctetString Value="0000600100FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.1.7.0.255 Puissance active import-->
//            <OctetString Value="0100010700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.2.7.0.255 Puissance active export-->
//            <OctetString Value="0100020700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.1.1.8.0.255 Energie active import totale-->
//            <OctetString Value="0101010800FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.1.2.8.0.255 Energie active export totale -->
//            <OctetString Value="0101020800FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.31.7.0.255 Courant instantané L1-->
//            <OctetString Value="01001F0700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.51.7.0.255 Courant instantané L2-->
//            <OctetString Value="0100330700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.71.7.0.255 Courant instantané L3-->
//            <OctetString Value="0100470700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.32.7.0.255 tension instantanée L1-->
//            <OctetString Value="0100200700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.52.7.0.255 tension instantanée L2-->
//            <OctetString Value="0100340700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//          <Structure Qty="04" >
//            <UInt16 Value="0003" />
//            <!--1.0.72.7.0.255 tension instantanée L3-->
//            <OctetString Value="0100480700FF" />
//            <Int8 Value="02" />
//            <UInt16 Value="0000" />
//          </Structure>
//        </Array>
//        <!--0.8.25.9.0.255-->
//        <OctetString Value="0008190900FF" />
//        <!--66681410-->
//        <OctetString Value="3636363831343130" />
//        <UInt32 Value="0000015A" />
//        <UInt32 Value="00000000" />
//        <UInt32 Value="003D1B04" />
//        <UInt32 Value="00000000" />
//        <UInt16 Value="0074" />
//        <UInt16 Value="000E" />
//        <UInt16 Value="0023" />
//        <UInt16 Value="00EB" />
//        <UInt16 Value="00EA" />
//        <UInt16 Value="00EA" />
//      </Structure>
//    </DataValue>
//  </NotificationBody>
//</DataNotification>

static const char *TAG = "pdu";

typedef enum {
    NULL_TYPE = 0x00,
    ARRAY_TYPE = 0x01,
    STRUCTURE_TYPE = 0x02,
    BOOLEAN_TYPE = 0x03,
    BIT_STRING_TYPE = 0x04,
    INT32_TYPE = 0x05,
    UINT32_TYPE = 0x06,
    OCTET_STRING_TYPE = 0x09,
    VISIBLE_STRING_TYPE = 0x0A,
    BCD_TYPE = 0x0D,
    INT8_TYPE = 0x0F,
    INT16_TYPE = 0x10,
    UINT8_TYPE = 0x11,
    UINT16_TYPE = 0x12,
    COMPACT_ARRAY_TYPE = 0x13,
    LONG64_TYPE = 0x14,
    UNSIGNED_LONG64_TYPE = 0x15,
    ENUM_TYPE = 0x16,
    FLOAT32_TYPE = 0x17,
    FLOAT64_TYPE = 0x18,
    DATE_TIME_TYPE = 0x19,
    DATE_TYPE = 0x1A,
    TIME_TYPE = 0x1B
} Types;

bool decode_item(const uint8_t *bytes, int size, int *pos) {
    if (*pos + 1 > size) {
        return false;
    }
    uint8_t type = bytes[*pos];
    ++*pos;
    switch (type) {
    case STRUCTURE_TYPE: {
        ESP_LOGI(TAG, "structure {");
        if (*pos + 1 > size) {
            return false;
        }
        const uint8_t quantity = bytes[*pos];
        ++*pos;
        for (uint8_t i = 0; i < quantity; ++i) {
            if (!decode_item(bytes, size, pos)) {
                return false;
            }
        }
        ESP_LOGI(TAG, "}");
        return true;
    }
    case ARRAY_TYPE: {
        ESP_LOGI(TAG, "array {");
        if (*pos + 1 > size) {
            return false;
        }
        const uint8_t quantity = bytes[*pos];
        ++*pos;
        for (uint8_t i = 0; i < quantity; ++i) {
            if (!decode_item(bytes, size, pos)) {
                return false;
            }
        }
        ESP_LOGI(TAG, "}");
        return true;
    }
    case UINT32_TYPE: {
        if (*pos + 4 > size) {
            return false;
        }
        const uint32_t value = (bytes[*pos] << 24) | (bytes[*pos + 1] << 16) | (bytes[*pos + 2] << 8) | bytes[*pos + 3];
        *pos += 4;
        ESP_LOGI(TAG, "value=%lu(uint32)", value);
        return true;
    }
    case UINT16_TYPE: {
        if (*pos + 2 > size) {
            return false;
        }
        const uint16_t value = bytes[*pos] << 8 | bytes[*pos + 1];
        *pos += 2;
        ESP_LOGI(TAG, "value=%u(uint16)", value);
        return true;
    }
    case UINT8_TYPE: {
        if (*pos + 1 > size) {
            return false;
        }
        const uint8_t value = bytes[*pos];
        ++*pos;
        ESP_LOGI(TAG, "value=%u(uint8)", value);
        return true;
    }
    case INT8_TYPE: {
        if (*pos + 1 > size) {
            return false;
        }
        const int8_t value = (int8_t)bytes[*pos];
        ++*pos;
        ESP_LOGI(TAG, "value=%d(int8)", value);
        return true;
    }
    case OCTET_STRING_TYPE: {
        if (*pos + 1 > size) {
            return false;
        }
        const uint8_t stringSize = bytes[*pos];
        ++*pos;
        if (*pos + stringSize > size) {
            return false;
        }
        *pos += stringSize;
        ESP_LOGI(TAG, "value=...(octetString%d)", stringSize);
        return true;
    }
    default:
        ESP_LOGW(TAG, "Unknown type: 0x%02x", type);
        return false;
    }
}

bool pdu_decode(const uint8_t *bytes, int size) {
    int pos = 0;
    pos += 1;  // unknown crap
    pos += 4;  // LongInvokeIdAndPriority
    pos += 1;  // unknown crap
    pos += 12; // DateTime

    if (!decode_item(bytes, size, &pos)) {
        return false;
    }
    return pos == size;
}
