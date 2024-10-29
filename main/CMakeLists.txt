idf_component_register(
    SRCS
        main.c
        led.c
        zigbee.c
        zigbee_light.c
        zigbee_meter.c
        zigbee_temperature.c
        zigbee_utils.c
        uart.c
        hdlc_fields.c
        hdlc_frame.c
        hdlc_packet.c
        dlms.c
        pdu.c
        buffer.c
        stream.c
    INCLUDE_DIRS ".")
