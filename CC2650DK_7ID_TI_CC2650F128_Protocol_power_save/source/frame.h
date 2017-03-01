/*
 * frame.h
 *
 *  Created on: 2016¦~5¤ë30¤é
 *      Author: 03565
 */

#ifndef INCLUDE_FRAME_H_
#define INCLUDE_FRAME_H_

#define START_BYTE_1        ('$')
#define START_BYTE_2        ('#')
#define START_BYTE_3        ('%')

enum frame_type {
    PRESERVE = 0,
    SYSTEM_START = 1,
    SYSTEM_INFO,
    SET_LORA_CONFIG,
    GET_LORA_CONFIG,
    TIME_SYNC,
    SET_SENSOR_SETTING,
    GET_SENSOR_SETTING,
    GET_SENSOR_VALUE,
    FIRMWARE_UPDATE,
    GET_LOG,
    CLEAR_LOG,
    ACK,
    SUCCESS,
    FAIL,
    DEV_DISTINGUISH,
    DISCONNECT,
	IGNORE_BROADCAST,
	GET_MESSAGE,
    MAX_COMMAND
};

#pragma pack(push)
#pragma pack(1)

struct header {
    uint8_t   dest_id[8];
    uint8_t   sour_id[8];
    uint16_t  section;
    uint16_t  sequence;
    uint16_t  type;             // The type of frame
    uint16_t  preserve;
    uint16_t  payload_len;      // length of payload
    uint16_t  checksum;
};

struct payload {
    uint16_t  checksum;
    uint8_t   data[0];
};


struct frame {
    uint8_t  start[3];
    uint8_t  group;

    struct header  header;
    struct payload payload;
};

#pragma pack(pop)

#endif /* INCLUDE_FRAME_H_ */
