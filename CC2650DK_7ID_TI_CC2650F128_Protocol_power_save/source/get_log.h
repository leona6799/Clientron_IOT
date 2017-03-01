/*
 * get_log.h
 *
 *  Created on: 2016¦~6¤ë14¤é
 *      Author: 03565
 */

#ifndef SOURCE_GET_LOG_H_
#define SOURCE_GET_LOG_H_

#pragma pack(push)
#pragma pack(1)

struct log_info {
    uint8_t  final;
    uint8_t  size;
    uint8_t  data[0];
};

#pragma pack(pop)

int get_log(struct frame *frame);
int clear_log(struct frame *frame);

#endif /* SOURCE_GET_LOG_H_ */
