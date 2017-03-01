/*
 * YGC160FS.c
 *
 *  Created on: 2016/08/08
 *      Author: 03468
 */

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/ADC.h>

#include "source/sensor_hub.h"

int YGC160FS_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size)
{
	uint32_t     voltage;

	if(! bus->bus_read(handle, address, &voltage, sizeof(uint32_t)))
		return -1;

	return System_snprintf(data, size, "Fan:%d", voltage);
}
