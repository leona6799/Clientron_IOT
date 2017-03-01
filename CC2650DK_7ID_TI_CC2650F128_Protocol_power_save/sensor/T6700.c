/*
 * T6700.c
 *
 *  Created on: 2016/07/29
 *      Author: 03468
 */

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>

#include "source/sensor_hub.h"

int CO2_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size)
{	
	uint8_t   CO2_command[] = {0x04, 0x13, 0x8B, 0x00, 0x01};
	uint8_t   CO2_buffer[4];
	int       CO2Value;
	
	if (! bus->bus_write(handle, address, CO2_command, sizeof(CO2_command)))
	    	return -1;

	Task_sleep(ONE_SECOND);

	if (! bus->bus_read(handle, address, CO2_buffer, sizeof(CO2_buffer)))
	    	return -1;

	CO2Value = (((int)CO2_buffer[2]) << 8 ) + CO2_buffer[3];

	return System_snprintf(data, size, "CO2:%d", CO2Value);
}
