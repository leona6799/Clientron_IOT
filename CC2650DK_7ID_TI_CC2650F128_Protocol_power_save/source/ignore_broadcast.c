/*
 * ignore_broadcast.c
 *
 *  Created on: 2016¦~12¤ë9¤é
 *      Author: 03565
 */

#include "sensor_hub.h"

int ignore_broadcast(struct frame *frame)
{
	hub.ignore_broadcast_timer = IGNORE_BROADCAST_TIME_OUT;

	if (hub.first_start)
		Clock_start(clkHandle);

	return EXECUTE_SUCCESS;
}
