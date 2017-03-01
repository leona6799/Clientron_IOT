/*
 * time_fxn.c
 *
 *  Created on: 2016¦~5¤ë31¤é
 *      Author: 03565
 */

#include <time.h>
#include <string.h>
#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/heaps/HeapMem.h>

#include "sensor_hub.h"

static int shine_count;

void timer_fxn(UArg arg0)
{
	if (hub.ignore_broadcast_timer <= 0) {
		Semaphore_post(Semaphore_handle(&hub.blink_smp));

		if (! hub.first_start)
			Semaphore_post(Semaphore_handle(&hub.timer_smp));

	} else {
    	set_pin(Board_Red_LED, 1);

    	if (--hub.ignore_broadcast_timer <= 0 && hub.first_start) {
    		set_pin(Board_Red_LED, 0);
    		Clock_stop(clkHandle);
    	}
    }
}

void blink_task(UArg arg0, UArg arg1)
{
    shine_count = 0;
    while (TRUE) {
        Semaphore_pend(Semaphore_handle(&hub.blink_smp), BIOS_WAIT_FOREVER);

        if (hub.blink) {
            if (++shine_count < 30) {
                set_pin(Board_Green_LED, !get_pin(Board_Green_LED));
                set_pin(Board_Red_LED,   !get_pin(Board_Red_LED));
            } else {
                set_pin(Board_Green_LED, 1);
                set_pin(Board_Red_LED,   0);

                hub.blink   = FALSE;
                shine_count = 0;

                if (hub.first_start)
                    Clock_stop(clkHandle);
            }
        //}else if (! hub.already_setting && hub.ignore_broadcast_timer <= 0) {  //Green LED will turn off
        }else if (hub.ignore_broadcast_timer <= 0) {  //Green LED will blinking
            set_pin(Board_Green_LED, !get_pin(Board_Green_LED));
        }

        Semaphore_reset(Semaphore_handle(&hub.blink_smp), 0);
    }
}

void timer_task(UArg arg0, UArg arg1)
{
    IArg       key;
    //uint8_t    log[EACH_LOG_LENGTH];
    //uint8_t   *ptr;
    int        i;
    //int        time_length;

    while (TRUE) {
        Semaphore_pend(Semaphore_handle(&hub.timer_smp), BIOS_WAIT_FOREVER);

#if 0
        {
            time_t     t;
            struct tm *tm;
        
            time(&t);
            tm = localtime(&t);
            time_length = System_snprintf((char *)log, sizeof(log), "%04d/%02d/%02d %02d:%02d:%02d", tm->tm_year + 1900,
                                          tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        }
        ptr = log + time_length;
#endif
#if AUTO_CATCH_WHEN_TIME_OUT
        {
            int length;
            key = GateHwi_enter(GateHwi_handle(&hub.socket_gate));
            for (i = 0; i < MAX_SOCKET; ++i) {
                if (hub.socket[i].sensor_id != NOT_CONNECT && hub.socket[i].catch_period != NONE_PERIOD) {
                    if (--hub.socket[i].countdown == 0) {
                        hub.socket[i].countdown = hub.socket[i].catch_period;
        
                        memcpy(ptr, "value->", 7);
                        length = catch_sensor_value('A' + i, hub.socket[i].sensor_id, ptr + 7, sizeof(log) - time_length - 7);
                        store_log(log, length + time_length + 7);
        
                        continue;
                    }
                }
            }
            GateHwi_leave(GateHwi_handle(&hub.socket_gate), key);
        }
#endif
        
        key = GateHwi_enter(GateHwi_handle(&hub.send_gate));

        for (i = 0; i < MAX_SEND_QUEUE; ++i) {
            if (! hub.send[i].flag)
                continue;

            if (--hub.send[i].countdown <= 0) {
                if (--hub.send[i].reply_times <= 0) {
                    hub.send[i].flag = FALSE;

#if STORE_LOG_WHEN_SEND_FAIL
                    int length;
                    struct frame *frame = (struct frame *)hub.send[i].reply;

                    if (frame->header.type != GET_LOG && frame->header.payload_len > 0) {
                        length = System_snprintf((char *)ptr, sizeof(log) - time_length, "type:%04x->", frame->header.type);

                        memcpy((void *)(ptr + length), (const void *)frame->payload.data, frame->header.payload_len - sizeof(struct payload));
                        length += time_length + frame->header.payload_len - sizeof(struct payload);

                        store_log(log, length);
                    }
#endif

                } else {
                	set_lora_mode(LORA_MODE_1);
                    hub.send[i].countdown = NO_RESPONSES_TIME;
                    write_lora((const void *)hub.send[i].reply, hub.send[i].reply_length, TRUE);
                }
            }
        }
        set_lora_mode(LORA_MODE_3);

        GateHwi_leave(GateHwi_handle(&hub.send_gate), key);
        Semaphore_reset(Semaphore_handle(&hub.timer_smp), 0);
    }
}
