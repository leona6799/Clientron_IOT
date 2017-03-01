/*
 * disconnect.c
 *
 *  Created on: 2016¦~7¤ë5¤é
 *      Author: 03565
 */

#include "sensor_hub.h"

int disconnect(struct frame *frame)
{
	int result;

    if (frame == NULL || frame->header.type != DISCONNECT)
        return PARAMETER_INVALID;

    result = spi_clear_everything();
    if (result < 0)
    	result |= DISCONNECT_FAIL;

    return result;
}
