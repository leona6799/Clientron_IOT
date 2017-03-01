/*
 * I2C.c
 *
 *  Created on: 2016¦~6¤ë22¤é
 *      Author: 03565
 */

#include <xdc/std.h>
#include <ti/drivers/I2C.h>

#include "sensor_hub.h"

void *open_i2c(void)
{
    I2C_Handle      handle;
    I2C_Params      params;

    I2C_Params_init(&params);

    handle = I2C_open(CC2650DK_7ID_I2C0, &params);

    return handle;
}

void close_i2c(void *handle)
{
    I2C_close((I2C_Handle)handle);
}

Bool read_i2c(void *handle, uint8_t address, void *buf, size_t size)
{
    I2C_Transaction i2cTransaction;

    i2cTransaction.writeBuf = NULL;
    i2cTransaction.writeCount = 0;
    i2cTransaction.readBuf = buf;
    i2cTransaction.readCount = size;
    i2cTransaction.slaveAddress = address;

    return I2C_transfer((I2C_Handle)handle, &i2cTransaction);
}

Bool write_i2c(void *handle, uint8_t address, void *buf, size_t size)
{
    I2C_Transaction i2cTransaction;

    i2cTransaction.writeBuf = buf;
    i2cTransaction.writeCount = size;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;
    i2cTransaction.slaveAddress = address;

    return I2C_transfer((I2C_Handle)handle, &i2cTransaction);
}
