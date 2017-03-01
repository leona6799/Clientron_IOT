/*
 * socket.h
 *
 *  Created on: 2016¦~5¤ë30¤é
 *      Author: 03565
 */

#ifndef INCLUDE_SENSOR_H_
#define INCLUDE_SENSOR_H_

#include <ti/sysbios/gates/GateHwi.h>

#include "frame.h"

#define MAX_SOCKET              6
#define NOT_CONNECT             0xFFFF
#define NONE_PERIOD             0
#define LOST_CONTACT_TIME       3

enum bus {
    I2C = 0,
    UART,
    SPI,
	ADC,
    MAX_BUS_TYPE
};

struct socket_t {
    uint16_t  sensor_id;
    uint16_t  bus_type;
    int32_t   catch_period;
    int32_t   countdown;
};

#pragma pack(push)
#pragma pack(1)

struct sensor_config {
    uint16_t  sensor_id;
    uint16_t  bus_type;
    int32_t   catch_period;
};

#pragma pack(pop)

int set_sensor_setting(struct frame *frame);
int get_sensor_setting(struct frame *frame);

#endif /* INCLUDE_SENSOR_H_ */
