/*
 * store_log.h
 *
 *  Created on: 2016¦~5¤ë25¤é
 *      Author: 03565
 */

#ifndef STORELOG_STORE_LOG_H_
#define STORELOG_STORE_LOG_H_

#include "source/sensor_setting.h"
#include "source/lora.h"

#define FIRST_TIME_USE		0
#define INIT_ANYWAY			1
#define EACH_LOG_LENGTH     128
#define NOT_FIRST_USE_SIGN    ((0x49 << 24) | (0x03 << 16) | (0x24 << 8) | 0x24)

#pragma pack(push)
#pragma pack(1)

struct log_entry {
    uint8_t    size;
    uint8_t    log[0];
};

struct config {
    uint32_t first_sign;
    uint8_t  edge_mac[8];

    Bool	 already_setting;

    struct lora_config   lora;         // define in lora.h
    struct sensor_config sensor[6];   //define sensor_setting.h
};

#pragma pack(pop)

extern int store_log(const uint8_t *log, size_t length);
extern int get_first_log(uint8_t *log, uint8_t *size, uint32_t *key);
extern int get_next_log(uint32_t *key, uint8_t *log, uint8_t *size);
extern int spi_clear_log(void);
extern int init_log_storage(int cond);
extern int spi_clear_everything(void);
extern int spi_get_config(struct config *config);
extern int spi_store_config(struct config *config);

#endif /* STORELOG_STORE_LOG_H_ */
