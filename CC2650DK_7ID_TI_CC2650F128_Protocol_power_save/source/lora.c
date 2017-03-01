/*
 * lora.c
 *
 *  Created on: 2016¦~6¤ë8¤é
 *      Author: 03565
 */

#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/gates/GateTask.h>
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/drivers/UART.h>

#include "sensor_hub.h"

void set_lora_mode(uint8_t mode)
{
    uint32_t p1, p2;
    static uint8_t pre_mode = LORA_MODE_1;

    if (pre_mode == mode)
        return;

    switch(mode) {
    case LORA_MODE_4:
        p1 = 1;
        p2 = 1;
        break;
    case LORA_MODE_3:  //sleep mode
        p1 = 0; //p1 = 1;
        p2 = 0;
        break;
    case LORA_MODE_2:  //wakeup mode
        p1 = 0;
        p2 = 1;
        break;
    case LORA_MODE_1:
    default:
        p1 = 0;
        p2 = 0;
        break;
    }

    set_pin(LoRa_P2, p2);
    set_pin(LoRa_P1, p1);

    if (pre_mode == LORA_MODE_4)
        Task_sleep(3 * ONE_SECOND);
    else
        Task_sleep(50 * ONE_SECOND / 1000);

    pre_mode = mode;
}

#if 0
static uint8_t *verify_lora_packet(uint8_t *packet, uint32_t length)
{
    uint8_t           *end;
    struct lora_resp  *resp;

    end = packet + length;
    while (packet < end) {
        resp = (struct lora_resp *)packet;
        if (resp->prefix == LORA_RESPONSE_PREFIX && resp->suffix == LORA_RESPONSE_SUFFIX)
            break;

        ++packet;
    }

    if (packet >= end)
        return NULL;

    return packet;
}

int obtain_lora_config(struct lora_resp *resp)
{
    IArg       key;
    int        size;
    uint8_t   *ptr;
    uint8_t    buf[LORA_BUFFER_LENGTH] = LORA_RAED_COMMAND;

    if (resp == NULL)
        return -1;

    key = GateHwi_enter(GateHwi_handle(&hub.lora_write_gate));
    Semaphore_pend(Semaphore_handle(&hub.lora_raed_smp), BIOS_WAIT_FOREVER);

    UART_readCancel(hub.lora);

    UART_close(hub.lora);
    set_lora_mode(LORA_MODE_4);
    hub.lora = open_lora(hub.lora_config.UART_rate, FALSE);

    UART_write(hub.lora, (void *)buf, LORA_SETTING_PREFIX_LENGTH);
    size = UART_read(hub.lora, (void *)buf, LORA_BUFFER_LENGTH);

    UART_close(hub.lora);
    set_lora_mode(LORA_MODE_2);
    hub.lora = open_lora(hub.lora_config.UART_rate, FALSE);

    Semaphore_post(Semaphore_handle(&hub.lora_raed_smp));
    GateHwi_leave(GateHwi_handle(&hub.lora_write_gate), key);

    ptr = verify_lora_packet(buf, size);

    if (ptr != NULL) {
        memcpy(resp, ptr, sizeof(struct lora_resp));
        return 0;
    }

    return -1;
}
#endif

int set_lora_config(struct frame *frame)
{
    int                 result;
    struct config       config;
    struct lora_config *lora;

    if (frame == NULL || frame->header.type != SET_LORA_CONFIG)
        return PARAMETER_INVALID;

    if (frame->header.payload_len < sizeof(struct payload) + sizeof(struct lora_config))
        return PAYLOAD_LEN_INVALID;

    lora = (struct lora_config *)frame->payload.data;

    if (lora->bitrate     > MAX_BITRATE             ||
        lora->transmit_Pw > MAX_ROLA_TRANSMIT_POWER ||
        lora->UART_rate   > MAX_UART_RATE           ||
        lora->wakeup_time > MAX_WAKE_UP) {
        return PAYLOAD_PARAMETER_INVALID;
    }

    result = spi_get_config(&config);
    if (result < 0)
        return result | SET_LORA_GET_CONFIG_ERROR;

    config.lora.group       = lora->group;
    config.lora.bitrate     = lora->bitrate;
    config.lora.transmit_Pw = lora->transmit_Pw;
    config.lora.UART_rate   = lora->UART_rate;
    config.lora.parity_bit  = lora->parity_bit;
    config.lora.wakeup_time = lora->wakeup_time;
    memcpy(config.lora.frequency, lora->frequency, sizeof(config.lora.frequency));

    result = spi_store_config(&config);
	if (result < 0)
		result |= SET_LORA_STORE_CONFIG_ERROR;

    return EXECUTE_SUCCESS;
}

int get_lora_config(struct frame *frame)
{
    if (frame == NULL || frame->header.type != GET_LORA_CONFIG)
        return PARAMETER_INVALID;

    send_reply(frame, frame->header.sequence + 1, frame->header.type, (const uint8_t *)&hub.lora_info, sizeof(struct lora_info));

    return EXECUTE_SUCCESS;
}

