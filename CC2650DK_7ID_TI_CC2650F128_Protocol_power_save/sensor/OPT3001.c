/*
 * OPT3001.c
 *
 *  Created on: 2016¦~6¤ë22¤é
 *      Author: 03565
 */

#include <math.h>
#include <xdc/runtime/System.h>

#include "source/sensor_hub.h"

Bool opt3001_config(struct bus_option *bus, void *handle, uint8_t address)
{
	Bool    success;
    uint8_t data[3];

    data[0] = 0x01;
    data[1] = 0xC2;
    data[2] = 0x10;

    success = bus->bus_write(handle, address, data, sizeof(data) / sizeof(uint8_t));

    if (success)
    	Task_sleep(ONE_SECOND / 10);

    return success;
}

int opt3001_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size)
{
    float    value;
    uint16_t lux, e, m;
    uint8_t rxbuf[2];
    uint8_t txbuf[1];

    lux      = 0;
    txbuf[0] = 0x01;
    while(TRUE) {
    	if (! bus->bus_write(handle, address, txbuf, sizeof(txbuf) / sizeof(uint8_t)))
    		return -1;

    	if (! bus->bus_read(handle, address, rxbuf, sizeof(rxbuf) / sizeof(uint8_t)))
    		return -1;

    	if ((rxbuf[1] & 0x80) == 0x80)
    		break;

    	if (++lux >= 20) {
    		return -1;
    	} else {
    		Task_sleep(ONE_SECOND / 10);
    	}
    }

    txbuf[0] = 0x00;
    if (! bus->bus_write(handle, address, txbuf, sizeof(txbuf) / sizeof(uint8_t)))
    	return -1;

    if (! bus->bus_read(handle, address, rxbuf, sizeof(rxbuf) / sizeof(uint8_t)))
    	return -1;

    lux = (rxbuf[0] << 8) | rxbuf[1];
    m = lux & 0x0FFF;
    e = (lux & 0xF000) >> 12;
    value = (float)m * (0.01 * exp2(e));

    return System_snprintf(data, size, "lux:%f", value);
}
