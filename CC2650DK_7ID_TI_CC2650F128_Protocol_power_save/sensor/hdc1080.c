/*
 * hdc1080.c
 *
 *  Created on: 2016¦~6¤ë22¤é
 *      Author: 03565
 */

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>

#include "source/sensor_hub.h"

Bool hdc1080_config(struct bus_option *bus, void *handle, uint8_t address)
{
    uint8_t data[3];

    data[0] = 0x02;
    data[1] = 0x10;
    data[2] = 0x00;

    return bus->bus_write(handle, address, data, sizeof(data) / sizeof(uint8_t));
}

int hdc1080_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size)
{
    uint16_t temp, humi;
    uint8_t rxbuf[4];
    uint8_t txbuf[1];


    txbuf[0] = 0x00;
    if (! bus->bus_write(handle, address, txbuf, sizeof(uint8_t)))
    	return -1;

    Task_sleep(ONE_SECOND / 10);

    if (! bus->bus_read(handle, address, rxbuf, sizeof(rxbuf) / sizeof(uint8_t)))
    	return -1;

    temp = (rxbuf[0] << 8) | rxbuf[1];
    temp = (((double)temp) / 65536) * 165 - 40;

    humi = (rxbuf[2] << 8) | rxbuf[3];
    humi = ((double)humi / 65536) * 100;

    return System_snprintf(data, size, "Temperature:%d Humidity:%d%%", temp, humi);
}
