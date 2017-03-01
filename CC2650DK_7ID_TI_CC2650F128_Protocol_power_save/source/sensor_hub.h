/*
 * sernor_note.h
 *
 *  Created on: 2016¦~6¤ë1¤é
 *      Author: 03565
 */

#ifndef INCLUDE_SENSOR_HUB_H_
#define INCLUDE_SENSOR_HUB_H_

#include <xdc/runtime/IHeap.h>

#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/gates/GateTask.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <ti/drivers/UART.h>

#include "Board.h"
#include "frame.h"
#include "lora.h"
#include "sensor_setting.h"
#include "system_info.h"
#include "system_start.h"
#include "receive_ack.h"
#include "time_sync.h"
#include "get_log.h"
#include "catch_value.h"
#include "dev_distinguish.h"
#include "disconnect.h"
#include "store_log/store_log.h"
#include "error_code.h"
#include "ignore_broadcast.h"

#define AUTO_CATCH_WHEN_TIME_OUT		0
#define STORE_LOG_WHEN_SEND_FAIL        0
#define SEND_DEBUG_MESSAGE_TO_UART      0
#define SEND_DEBUG_MESSAGE_TO_SPI       1
#define MUTIL_TASK                      0
#define DEBUG_WITH_J_TAG                0
#define LORA_WAKE_UP_TIME               0x0b

#if MUTIL_TASK
#define TASKSTACKSIZE           	3072
#define MAIN_TASKSTACKSIZE      	1024
#define TIME_TASKSTACKSIZE      	1024
#define BLINK_TASKSTACKSIZE     	256
#else
#define MAIN_TASKSTACKSIZE      	3072
#define TIME_TASKSTACKSIZE      	1024
#define BLINK_TASKSTACKSIZE    	 	256
#endif

#define MAX_RECEIVE_BUFFER      	1
//#define MAX_SEND_BUFFER            (MAX_RECEIVE_BUFFER + 2)
#define MAX_SEND_QUEUE          	(MAX_RECEIVE_BUFFER + 2)
#define BUFFER_NOT_FOUND        	-1
#define TERMINATED_FLAG         	0xFFFFFFFF
#define OCCUPIED_FLAG           	1
#define VACANT_FLAG             	0
#define NO_RESPONSES_TIME       	5
#define WAIT_ACK_TIME           	3
#define BROADCAST_MAC_ADDRESS   	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define ONE_SECOND              	(1000000 / Clock_tickPeriod)
#define REPEAT_ACK              	1
#define IGNORE_BROADCAST_TIME_OUT	(3 * 60)   // 5 minutes

struct wait_ack {
    Bool       flag;
    int32_t    reply_times;
    int32_t    countdown;
    uint32_t   reply_length;
    uint8_t    reply[MAX_ROLA_PACKAGE_LENGTH];
};

#if MUTIL_TASK
struct rec_space {
    Task_Struct   task_Struct;
    uint8_t       rec_buf[MAX_ROLA_PACKAGE_LENGTH];
    uint8_t       rec_stack[TASKSTACKSIZE];
    uint32_t      flag;
};
#else
#undef MAX_RECEIVE_BUFFER
#define MAX_RECEIVE_BUFFER		1

#undef MAX_SEND_QUEUE
#define MAX_SEND_QUEUE          (MAX_RECEIVE_BUFFER + 2)
#endif

struct sensor_hub {
    Bool             blink;
    int              ignore_broadcast_timer;

    Semaphore_Struct lora_smp;
    GateHwi_Struct   lora_write_gate;
    UART_Handle      lora;

    Bool             already_setting;

    uint8_t          edge_mac[8];
    struct lora_info lora_info;

    Bool             first_start;

    Semaphore_Struct timer_smp;
    Semaphore_Struct blink_smp;

#if MUTIL_TASK
    Semaphore_Struct task_reclaim_smp;
    GateHwi_Struct   lora_rec_gate;
    struct rec_space rec_entity[MAX_RECEIVE_BUFFER];
#endif

    GateHwi_Struct    send_gate;
    struct wait_ack   send[MAX_SEND_QUEUE];

    GateHwi_Struct    bus_gate[MAX_BUS_TYPE];

    GateHwi_Struct   socket_gate;
    struct socket_t  socket[MAX_SOCKET];
};

inline void set_pin(PIN_Id pinId, uint_t val);
inline uint_t get_pin(PIN_Id pinId);

extern PIN_Handle        intPinHandle;
extern Clock_Handle      clkHandle;
extern struct sensor_hub hub;
//extern Queue_Handle      wait_queue;
//extern GateHwi_Struct    wait_queue_gate;

#if SEND_DEBUG_MESSAGE_TO_UART || SEND_DEBUG_MESSAGE_TO_SPI
void send_message(char *prefix);
void send_message_data(char *data, size_t size);
void send_message_c(char C);
void send_message_value(char *prefix, uint32_t value);
void send_message_only_value(uint32_t value);
#else
#define send_message_c(a)
#define send_message_data(a, b)
#define send_message(a)
#define send_message_value(a, b)
#define send_message_only_value(a)
#endif

void timer_fxn(UArg arg0);
void blink_task(UArg arg0, UArg arg1);
void timer_task(UArg arg0, UArg arg1);
void receive_fxn(UArg arg0, UArg arg1);

#if MUTIL_TASK
void reclaim_fxn(UArg arg0, UArg arg1);
#endif

void send_ack(struct frame *frame);
void process_frame(UArg arg0, UArg arg1);
void send_reply(struct frame *frame, uint16_t sequence, uint16_t type, const uint8_t *data, uint32_t size);
void write_lora(const void *buffer, size_t size, Bool disable);

uint16_t calculate_crc(uint8_t *buf, size_t size);

#endif /* INCLUDE_SENSOR_HUB_H_ */
