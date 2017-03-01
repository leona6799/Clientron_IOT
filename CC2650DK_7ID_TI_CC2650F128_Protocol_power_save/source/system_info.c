/*
 * system_info.c
 *
 *  Created on: 2016¦~6¤ë8¤é
 *      Author: 03565
 */
#include <string.h>

#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Types.h>

#include "sensor_hub.h"
#include "system_info.h"

int system_info(struct frame *frame)
{
    struct system_info    info;
    Types_FreqHz          freq;

    if (frame == NULL || frame->header.type != SYSTEM_INFO)
        return PARAMETER_INVALID;

    memset((void *)&info, 0, sizeof(struct system_info));

    strncpy((char *)&info.mcu[0], MCU_TYPE, sizeof(info.mcu));
    strncpy((char *)&info.spi[0], SPI_TYPE, sizeof(info.spi));

    memcpy(info.lora_mac, hub.lora_info.mac, sizeof(info.lora_mac));
    strncpy((char *)info.firmware_version, FIRMWARE_VERSION, sizeof(info.firmware_version));

     BIOS_getCpuFreq(&freq);
     info.cpu_freq = freq.lo;

     send_reply(frame, frame->header.sequence + 1, frame->header.type, (uint8_t *)&info, sizeof(struct system_info));

     return EXECUTE_SUCCESS;
}
