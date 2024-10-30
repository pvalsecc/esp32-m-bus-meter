# esp32-m-bus-meter

A MBus -> ZigBee bridge on ESP32-H2 for the  Landis&Gyr type E450 or E350 with the AD-FU MODULE.
Meter used by the Romande Energie in Switzerland which is using a DLMS/COSEM protocol.

## Hardware requirement

* An [esp32-h2 devkit board](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32h2/esp32-h2-devkitm-1/user_guide.html#getting-started)
* An [MBus to TTL slave adapter](https://www.aliexpress.com/item/1005006030830638.html)

## Useful links

* [Eliot's implementation for a different meter](https://github.com/EliotFerragni/smart-meter-p1-reader)
* [ESP8266 / ESP32 to MQTT](https://github.com/alekslt/HANToMQTT)
* [ZigBee Cluster Library Specification](https://zigbeealliance.org/wp-content/uploads/2019/12/07-5123-06-zigbee-cluster-library-specification.pdf)
* [COSEM Indentification System and Interface Objects](https://www.cs.ru.nl/~marko/onderwijs/bss/SmartMeter/Excerpt_BB7.pdf)

## PDU format

Here is what [Gurux DLMS Translator](https://www.gurux.fi/GuruxDLMSTranslator) finds in a re-assembled frame at the PDU level:

```xml
<DataNotification>
  <LongInvokeIdAndPriority Value="004B10AB" />
  <!--2024-10-27 07:35:45-->
  <DateTime Value="07E80A1B0707232DFF800000" />
  <NotificationBody>
    <DataValue>
      <Structure Qty="0D" >
        <Array Qty="0D" >
          <Structure Qty="04" >
            <UInt16 Value="0028" />
            <!--0.8.25.9.0.255-->
            <OctetString Value="0008190900FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0028" />
            <!--0.8.25.9.0.255-->
            <OctetString Value="0008190900FF" />
            <Int8 Value="01" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0001" />
            <!--0.0.96.1.0.255-->
            <OctetString Value="0000600100FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.0.1.7.0.255-->
            <OctetString Value="0100010700FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.0.2.7.0.255-->
            <OctetString Value="0100020700FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.1.1.8.0.255-->
            <OctetString Value="0101010800FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.1.2.8.0.255-->
            <OctetString Value="0101020800FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.0.31.7.0.255-->
            <OctetString Value="01001F0700FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.0.51.7.0.255-->
            <OctetString Value="0100330700FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.0.71.7.0.255-->
            <OctetString Value="0100470700FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.0.32.7.0.255-->
            <OctetString Value="0100200700FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.0.52.7.0.255-->
            <OctetString Value="0100340700FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
          <Structure Qty="04" >
            <UInt16 Value="0003" />
            <!--1.0.72.7.0.255-->
            <OctetString Value="0100480700FF" />
            <Int8 Value="02" />
            <UInt16 Value="0000" />
          </Structure>
        </Array>
        <!--0.8.25.9.0.255-->
        <OctetString Value="0008190900FF" />
        <!--66681421-->
        <OctetString Value="3636363831343231" />
        <UInt32 Value="0000015A" />
        <UInt32 Value="00000000" />
        <UInt32 Value="003D1B04" />
        <UInt32 Value="00000000" />
        <UInt16 Value="0074" />
        <UInt16 Value="000E" />
        <UInt16 Value="0023" />
        <UInt16 Value="00EB" />
        <UInt16 Value="00EA" />
        <UInt16 Value="00EA" />
      </Structure>
    </DataValue>
  </NotificationBody>
</DataNotification>
```
