/*
 * hardware.h
 *
 *  Created on: 2016¦~6¤ë8¤é
 *      Author: 03565
 */

#ifndef INCLUDE_SYSTEM_INFO_H_
#define INCLUDE_SYSTEM_INFO_H_

#include <xdc/std.h>

#include "error_code.h"
#include "sensor_setting.h"

#define MCU_TYPE            "CC2640"
#define SPI_TYPE            "W25X20CL"
#define FIRMWARE_VERSION    "00.01.06"

#pragma pack(push)
#pragma pack(1)

struct system_info {
    uint8_t   mcu[12];
    uint32_t  cpu_freq;
    uint8_t   lora_mac[8];
    uint8_t   firmware_version[12];
    uint8_t   spi[12];
};

#pragma pack(pop)

int system_info(struct frame *frame);

#endif /* INCLUDE_SYSTEM_INFO_H_ */
