/*
 * receive_ack.c
 *
 *  Created on: 2016¦~6¤ë14¤é
 *      Author: 03565
 */

#include <xdc/runtime/Memory.h>

#include "sensor_hub.h"

int receive_ack(struct frame *frame)
{
    int    i;
    IArg   key;
    struct frame *reply;

    if (frame == NULL || frame->header.type != ACK)
        return PARAMETER_INVALID;

    key = GateHwi_enter(GateHwi_handle(&hub.send_gate));

    for (i = 0; i < MAX_SEND_QUEUE; ++i) {
        reply = (struct frame *)hub.send[i].reply;

        if (hub.send[i].flag &&
            frame->header.section  == reply->header.section &&
            frame->header.sequence == reply->header.sequence)
            hub.send[i].flag = FALSE;
    }

    GateHwi_leave(GateHwi_handle(&hub.send_gate), key);

    return EXECUTE_SUCCESS;
}
