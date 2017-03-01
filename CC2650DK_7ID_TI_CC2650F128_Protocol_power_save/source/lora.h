/*
 * lora.h
 *
 *  Created on: 2016¦~5¤ë31¤é
 *      Author: 03565
 */

#ifndef INCLUDE_LORA_H_
#define INCLUDE_LORA_H_

#define MAX_ROLA_TRANSMIT_POWER			7
#define MAX_ROLA_PACKAGE_LENGTH			256
#define LORA_BUFFER_LENGTH				30
#define LORA_SETTING_PREFIX_LENGTH		7
#define LORA_SETTING_PREFIX 			{0xFF, 0x4C, 0xCF, 0x52, 0xA1, 0x57, 0xF1}
#define LORA_RAED_COMMAND				{0xFF, 0x4C, 0xCF, 0x52, 0xA1, 0x52, 0xF0}
//#define LORA_DEFAULT_SETTING			{{0x0D, 0xF6, 0x38}, 0x00, 0x03, 0x07, 0x03, 0x00, 0x0b}  //915000 kHz
//#define LORA_DEFAULT_SETTING          {{0x0E, 0x1D, 0x48}, 0x00, 0x03, 0x07, 0x03, 0x00, 0x0b}  //925000 kHz
#define LORA_DEFAULT_SETTING			{{0x0E, 0x44, 0x58}, 0x00, 0x03, 0x07, 0x03, 0x00, 0x0b}  //935000 kHz
//#define LORA_DEFAULT_SETTING            {{0x0E, 0x6B, 0x68}, 0x00, 0x03, 0x07, 0x03, 0x00, 0x05}  //945000 KHz
//#define LORA_DEFAULT_SETTING          {{0x0F, 0x07, 0xA8}, 0x00, 0x03, 0x07, 0x03, 0x00, 0x05}  //985000 KHz
//#define LORA_DEFAULT_SETTING          {{0x0E, 0x50, 0x10}, 0x00, 0x03, 0x07, 0x03, 0x00, 0x05}  //938000 KHz
#define LORA_RESPONSE_PREFIX 			0x24
#define LORA_RESPONSE_SUFFIX 			0x21
#define LORA_MODE_1						1
#define LORA_MODE_2						2
#define LORA_MODE_3						3
#define LORA_MODE_4						4

enum Bit_Rate {
    _810BPS = 0,
    _1460BPS,
    _2600BPS,
    _4560BPS,
    _9110BPS,
    _18230BPS,
    MAX_BITRATE
};

enum UART_Rate {
    _1200BPS = 0,
    _2400BPS,
    _4800BPS,
    _9600BPS,
    _19200BPS,
    _38400BPS,
    _57600BPS,
    MAX_UART_RATE
};

enum wake_up {
    _50MS = 0,
    _100MS,
    _200MS,
    _400MS,
    _600MS,
    _1000MS,
    _1500MS,
    _2000MS,
    _2500MS,
    _3000MS,
    _4000MS,
    _5000MS,
    MAX_WAKE_UP
};

enum parity_bit {
    NONE_PARITY = 0,
    ODD_PARITY,
    EVEN_PARITY,
    MAX_PARITY_BIT
};

#pragma pack(push)
#pragma pack(1)

struct lora_info {
    uint8_t    module[4];
    uint8_t    version[7];
    uint8_t    mac[8];
    uint8_t    group;
    uint8_t    frequency[3];
    uint8_t    bitrate;
    uint8_t    transmit_Pw;
    uint8_t    UART_rate;
    uint8_t    parity_bit;
    uint8_t    wakeup_time;
};

struct lora_resp {
    uint8_t    prefix;

    struct lora_info info;

    uint8_t    suffix;
};

struct lora_config {
    uint8_t    frequency[3];
    uint8_t    group;
    uint8_t    bitrate;
    uint8_t    transmit_Pw;
    uint8_t    UART_rate;
    uint8_t    parity_bit;
    uint8_t    wakeup_time;
};

#pragma pack(pop)

void set_lora_mode(uint8_t mode);
int set_lora_config(struct frame *frame);
int get_lora_config(struct frame *frame);

#endif /* INCLUDE_LORA_H_ */
