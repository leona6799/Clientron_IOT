/*
 * sensor.h
 *
 *  Created on: 2016¦~6¤ë22¤é
 *      Author: 03565
 */

#ifndef SOURCE_SENSOR_FXN_H_
#define SOURCE_SENSOR_FXN_H_

Bool opt3001_config(struct bus_option *bus, void *handle, uint8_t address);
int  opt3001_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size);

Bool hdc1080_config(struct bus_option *bus, void *handle, uint8_t address);
int  hdc1080_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size);

int CO2_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size);
int DO_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size);

int YGC160FS_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size);

int MQ137_get_value(struct bus_option *bus, void *handle, uint8_t address, void *data, size_t size);

#endif /* SOURCE_SENSOR_FXN_H_ */
