/*
 * catch_value.h
 *
 *  Created on: 2016¦~5¤ë31¤é
 *      Author: 03565
 */

#ifndef INCLUDE_CATCH_VALUE_H_
#define INCLUDE_CATCH_VALUE_H_

struct bus_option {
    uint16_t      bus_type;

    void *(*bus_open) (void);
    Bool  (*bus_read)  (void *handle, uint8_t address, void *data, size_t size);
    Bool  (*bus_write) (void *handle, uint8_t address, void *data, size_t size);
    void  (*bus_close) (void *handle);
};

struct sensor_option {
    uint16_t    sensor_id;
    uint16_t    bus_type;
    uint8_t     address;

    Bool (*config)    (struct bus_option *bus, void *handle, uint8_t address);
    int  (*get_value) (struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size);
};

#pragma pack(push)
#pragma pack(1)

struct get_value {
    uint8_t   socket_id;
    uint16_t  sensor_id;
    uint16_t  length;
    uint8_t   data[0];
};

#pragma pack(pop)

int get_sensor_value(struct frame *frame);
int catch_sensor_value(uint8_t socket_id, uint16_t sensor_id, uint8_t *buf, size_t *length);

#endif /* INCLUDE_CATCH_VALUE_H_ */
