/*
 * socket.c
 *
 *  Created on: 2016¦~6¤ë8¤é
 *      Author: 03565
 */

#include <source/sensor_setting.h>
#include <xdc/std.h>
#include <ti/sysbios/gates/GateHwi.h>

#include "sensor_hub.h"

int set_sensor_setting(struct frame *frame)
{
	int result;
    int i;

    struct sensor_config  *sensor;
    struct config          config;

    if (frame == NULL || frame->header.type != SET_SENSOR_SETTING)
        return PARAMETER_INVALID;

    if (frame->header.payload_len < (sizeof(struct sensor_config) * MAX_SOCKET) + sizeof(struct payload))
        return PAYLOAD_LEN_INVALID;

    result = spi_get_config(&config);
    if (result < 0)
        return result | SET_SENSOR_GET_CONFIG_ERROR;

    sensor = (struct sensor_config *)frame->payload.data;
    IArg key = GateHwi_enter(GateHwi_handle(&hub.socket_gate));

    for (i = 0; i < MAX_SOCKET; ++i) {
        if (sensor->sensor_id != NOT_CONNECT) {
            config.sensor[i].sensor_id    = sensor->sensor_id;
            config.sensor[i].bus_type     = sensor->bus_type;
            config.sensor[i].catch_period = sensor->catch_period;

            hub.socket[i].sensor_id    = sensor->sensor_id;
            hub.socket[i].bus_type     = sensor->bus_type;
            hub.socket[i].catch_period = sensor->catch_period;

            if (sensor->catch_period != NONE_PERIOD)
                hub.socket[i].countdown = sensor->catch_period * LOST_CONTACT_TIME;
            else
                hub.socket[i].countdown = 0;
        } else {
            config.sensor[i].sensor_id    = NOT_CONNECT;
            config.sensor[i].bus_type     = MAX_BUS_TYPE;
            config.sensor[i].catch_period = NONE_PERIOD;

            hub.socket[i].sensor_id    = NOT_CONNECT;
            hub.socket[i].bus_type     = MAX_BUS_TYPE;
            hub.socket[i].catch_period = NONE_PERIOD;
            hub.socket[i].countdown    = 0;
        }

        ++sensor;
    }

    GateHwi_leave(GateHwi_handle(&hub.socket_gate), key);

    hub.already_setting = TRUE;
    config.already_setting = TRUE;

    result = spi_store_config(&config);
	if (result < 0)
		result |= SET_SENSOR_STORE_CONFIG_ERROR;

    return result;
}

int get_sensor_setting(struct frame *frame)
{
    int i;
    struct sensor_config  sensor[MAX_SOCKET];

    if (frame == NULL || frame->header.type != GET_SENSOR_SETTING)
        return PARAMETER_INVALID;

    for (i = 0; i < MAX_SOCKET; ++i) {
        sensor[i].sensor_id    = hub.socket[i].sensor_id;
        sensor[i].bus_type     = hub.socket[i].bus_type;
        sensor[i].catch_period = hub.socket[i].catch_period;
    }

    send_reply(frame, frame->header.sequence + 1, frame->header.type, (uint8_t *)sensor, sizeof(struct sensor_config) * MAX_SOCKET);

    return EXECUTE_SUCCESS;
}
