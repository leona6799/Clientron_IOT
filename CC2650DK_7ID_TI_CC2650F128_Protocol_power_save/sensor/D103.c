/*
 * D103.c
 *
 *  Created on: 2016/08/03
 *      Author: 03468
 */

#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>

#include "source/sensor_hub.h"

int DO_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size)
{	
	uint8_t   tx[1];
	uint8_t   rx[14];
	
	tx[0] = 'r';
	if (! bus->bus_write(handle, address, tx, sizeof(tx)))
    	return -1;

	Task_sleep(ONE_SECOND);

	do {
		if (! bus->bus_read(handle, address, rx, sizeof(rx)))
			return -1;

		if (rx[0] == 0xFE)
		    Task_sleep(ONE_SECOND / 10);
	} while (rx[0] == 0xFE);

	if (rx[0] != 0x01)
	    return -1;

	return System_snprintf(data, size, "DO:%s", &rx[1]);
}
