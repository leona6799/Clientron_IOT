/*
 * get_log.c
 *
 *  Created on: 2016¦~6¤ë14¤é
 *      Author: 03565
 */

#include "sensor_hub.h"

int get_log(struct frame *frame)
{
    int      result;
    uint32_t key;
    uint16_t sequence;
    uint8_t  log[sizeof(struct log_info) + EACH_LOG_LENGTH];

    struct log_info *info;

    if (frame == NULL || frame->header.type != GET_LOG)
        return PARAMETER_INVALID;

    info       = (struct log_info *)log;
    info->size = EACH_LOG_LENGTH;
    result     = get_first_log(info->data, &info->size, &key);
    if (result != EXECUTE_SUCCESS && result != FLASH_LOG_LAST) {
		return GET_LOG_FIRST_ERROR | result;
    }

    sequence = frame->header.sequence;
    while (result == EXECUTE_SUCCESS || result == FLASH_LOG_LAST) {
        //info->final = (result == FLASH_LOG_LAST ? 1 : 0);
        //send_reply(frame, ++sequence, frame->header.type, (const uint8_t *)info, sizeof(struct log_info) + info->size);
    	info->data[info->size++] = '#';
    	write_lora((const void *)info->data, info->size, FALSE);

        if (result == FLASH_LOG_LAST)
            break;

        info->size = EACH_LOG_LENGTH;
        result = get_next_log(&key, info->data, &info->size);

		if (result != EXECUTE_SUCCESS && result != FLASH_LOG_LAST)
			return GET_LOG_NEXT_ERROR | result;
    }

    return EXECUTE_SUCCESS;
}

int clear_log(struct frame *frame)
{
	int result;

    if (frame == NULL || frame->header.type != CLEAR_LOG)
        return PARAMETER_INVALID;

    result = spi_clear_log();
    if (result < 0)
    	result |= CLEAR_LOG_CLEAR_ERROR;

    return result;
}
