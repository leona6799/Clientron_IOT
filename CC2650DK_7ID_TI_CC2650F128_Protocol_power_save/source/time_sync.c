/*
 * time_sync.c
 *
 *  Created on: 2016¦~6¤ë14¤é
 *      Author: 03565
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ti/sysbios/hal/Seconds.h>
#include <xdc/runtime/System.h>

#include "source/sensor_hub.h"

int time_sync(struct frame *frame)
{
    uint32_t  second;

    if (frame == NULL || frame->header.type != TIME_SYNC)
        return PARAMETER_INVALID;

    if (frame->header.payload_len < sizeof(struct payload) + sizeof(uint32_t))
        return PAYLOAD_LEN_INVALID;

    second = *((uint32_t *)frame->payload.data);

    if (second > 0)
        Seconds_set(second);
    else
    	return PAYLOAD_PARAMETER_INVALID;

    return EXECUTE_SUCCESS;
}
