/*
 * dev_distinguish.c
 *
 *  Created on: 2016¦~6¤ë29¤é
 *      Author: 03565
 */

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include "sensor_hub.h"

int dev_distinguish(struct frame *frame)
{
    set_pin(Board_Green_LED, 0);
    set_pin(Board_Red_LED,   1);

    hub.blink = TRUE;

    if (hub.first_start)
        Clock_start(clkHandle);

    return EXECUTE_SUCCESS;
}
