/*
 * I2C.h
 *
 *  Created on: 2016¦~6¤ë22¤é
 *      Author: 03565
 */

#ifndef SOURCE_I2C_H_
#define SOURCE_I2C_H_

void *open_i2c(void);
void close_i2c (void *handle);
Bool read_i2c(void *handle, uint8_t address, void *buf, size_t size);
Bool write_i2c(void *handle, uint8_t address, void *buf, size_t size);


#endif /* SOURCE_I2C_H_ */
