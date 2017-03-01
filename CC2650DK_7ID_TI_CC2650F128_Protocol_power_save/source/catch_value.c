/*
 * catch_value.c
 *
 *  Created on: 2016¦~6¤ë22¤é
 *      Author: 03565
 */

#include "sensor_hub.h"
#include "I2C.h"
#include "sensor_id.h"
#include "sensor_fxn.h"
#include "ADC.h"

static struct bus_option buses[] = {
        {I2C,   open_i2c,   read_i2c,   write_i2c,   close_i2c},
//        {UART,  open_uart,  read_uart,  write_uart,  close_uart},
//        {SPI,   open_spi,   read_spi,   write_spi,   close_spi},
		{ADC,   open_adc,   read_adc,   write_adc,   close_adc},
        {MAX_BUS_TYPE, NULL, NULL, NULL, NULL}
};

static struct sensor_option sensors[] = {
        {OPT3001,       I2C, 0x44, opt3001_config, opt3001_get_value},
        {HDC1080,       I2C, 0x40, hdc1080_config, hdc1080_get_value},
		{T6700,         I2C, 0x15, NULL,           CO2_get_value},
		{D103,          I2C, 0x61, NULL,           DO_get_value},
		{YGC160FS,      ADC, 0x00, NULL,           YGC160FS_get_value},
		{MQ137,         ADC, 0x00, NULL,           MQ137_get_value},
        {MAX_SENSOR_ID, MAX_BUS_TYPE, 0xFF, NULL, NULL}
};

int catch_sensor_value(uint8_t socket_id, uint16_t sensor_id, uint8_t *buf, size_t *length)
{
    IArg                  key;
    int                   len;
    int                   result;
    void                 *handle;
    struct get_value     *reply;
    struct bus_option    *bus;
    struct sensor_option *sensor;

    if (buf == NULL || *length == 0)
    	return PARAMETER_INVALID;

    for (sensor = sensors; sensor->sensor_id != MAX_SENSOR_ID; ++sensor) {
        if (sensor_id == sensor->sensor_id)
            break;
    }

    if (sensor->sensor_id == MAX_SENSOR_ID)
        return SENSOR_ID_NOT_FOUND;

    for (bus = buses; bus->bus_type != MAX_BUS_TYPE; ++bus) {
        if (bus->bus_type == sensor->bus_type)
            break;
    }

    if (bus->bus_type == MAX_BUS_TYPE)
    	return SENSOR_BUS_NOT_FOUND;

    result = EXECUTE_SUCCESS;
    reply  = (struct get_value *)buf;
    len    = *length - sizeof(struct get_value);

    key = GateHwi_enter(GateHwi_handle(&hub.bus_gate[sensor->bus_type]));

    handle = bus->bus_open();
    if (handle == NULL) {
    	result = BUS_OPEN_FAIL;
    	goto Exit;
    }

    if (sensor->config != NULL && ! sensor->config(bus, handle, sensor->address)) {
    	bus->bus_close(handle);

    	result = SENSOR_CONFIG_FAIL;
    	goto Exit;
    }

    len = sensor->get_value(bus, handle, sensor->address, reply->data, len);

    bus->bus_close(handle);
    if (len < 0) {
    	result = GET_VALUE_FAIL;
    	goto Exit;
    }

    *length = sizeof(struct get_value) + len;
    reply->socket_id = socket_id;
    reply->sensor_id = sensor_id;
    reply->length    = (uint16_t)len;

    result = EXECUTE_SUCCESS;

Exit:
	GateHwi_leave(GateHwi_handle(&hub.bus_gate[sensor->bus_type]), key);
	return result;
}


int get_sensor_value(struct frame *frame)
{
	int      result;
    size_t   size;
    uint8_t  buf[128];

    struct get_value *request;

    if (frame == NULL || frame->header.type != GET_SENSOR_VALUE)
        return PARAMETER_INVALID;

    if (frame->header.payload_len < sizeof(struct get_value) + sizeof(struct payload))
        return PAYLOAD_LEN_INVALID;

    request = (struct get_value *)frame->payload.data;
    if (hub.socket[request->socket_id - 'A'].sensor_id != request->sensor_id)
        return PAYLOAD_PARAMETER_INVALID;

#if AUTO_CATCH_WHEN_TIME_OUT
    {
        IArg key = GateHwi_enter(GateHwi_handle(&hub.socket_gate));
        hub.socket[request->socket_id - 'A'].countdown = hub.socket[request->socket_id - 'A'].catch_period;
        GateHwi_leave(GateHwi_handle(&hub.socket_gate), key);
    }
#endif

    size = sizeof(buf);
    result = catch_sensor_value(request->socket_id, request->sensor_id, buf, &size);

    if (result == EXECUTE_SUCCESS)
    	send_reply(frame, frame->header.sequence + 1, frame->header.type, (const uint8_t *)buf, size);

    return result;
}
