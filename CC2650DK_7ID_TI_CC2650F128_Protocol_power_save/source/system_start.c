/*
 * system_start.c
 *
 *  Created on: 2016¦~6¤ë13¤é
 *      Author: 03565
 */

#include <ti/sysbios/knl/Clock.h>

#include "sensor_hub.h"

int system_start(struct frame *frame)
{
	int    result;
    int    i;
    IArg   key;

    struct config config;

    if (frame == NULL || frame->header.type != SYSTEM_START)
        return PARAMETER_INVALID;

    result = init_log_storage(FIRST_TIME_USE);
    if (result < 0)
    	return result | START_INIT_LOG_ERROR;

    key = GateHwi_enter(GateHwi_handle(&hub.socket_gate));
    for (i = 0; i < MAX_SOCKET; ++i) {
        hub.socket[i].sensor_id    = NOT_CONNECT;
        hub.socket[i].catch_period = NONE_PERIOD;
    }
    GateHwi_leave(GateHwi_handle(&hub.socket_gate), key);

    result = spi_get_config(&config);
    if (result < 0)
    	return result | START_GET_CONFIG_ERROR;

    memcpy(hub.edge_mac,    frame->header.sour_id, sizeof(hub.edge_mac));
    memcpy(config.edge_mac, frame->header.sour_id, sizeof(config.edge_mac));

    config.lora.group       = hub.lora_info.group;
    config.lora.bitrate     = hub.lora_info.bitrate;
    config.lora.transmit_Pw = hub.lora_info.transmit_Pw;
    config.lora.UART_rate   = hub.lora_info.UART_rate;
    config.lora.parity_bit  = hub.lora_info.parity_bit;
    config.lora.wakeup_time = hub.lora_info.wakeup_time;
    memcpy(config.lora.frequency, hub.lora_info.frequency, sizeof(config.lora.frequency));

    for (i = 0; i < MAX_SOCKET; ++i) {
        config.sensor[i].sensor_id    = NOT_CONNECT;
        config.sensor[i].catch_period = NONE_PERIOD;
    }

    result = spi_store_config(&config);
    if (result < 0)
    	result |= START_STORE_CONFIG_ERROR;

    return result;
}
