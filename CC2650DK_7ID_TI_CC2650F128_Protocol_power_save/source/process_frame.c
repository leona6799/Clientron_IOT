/*
 * process_frame.c
 *
 *  Created on: 2016年6月6日
 *      Author: 03565
 */

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Seconds.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <driverlib/sys_ctrl.h>

#include "sensor_hub.h"

struct command {
    enum frame_type  type;
    Bool             report;
    Bool             reboot;

    int (*command_fxn) (struct frame *frame);  //函式指標
};

//
// TODO :
//
static struct command commands[] = {
	  // Freame type            Report     Reboot     Function name
		{SYSTEM_START,          TRUE,      TRUE,      system_start        },
        {SYSTEM_INFO,           FALSE,     FALSE,     system_info         },
        {SET_LORA_CONFIG,       TRUE,      TRUE,      set_lora_config     },
        {GET_LORA_CONFIG,       FALSE,     FALSE,     get_lora_config     },
        {TIME_SYNC,             TRUE,      FALSE,     time_sync           },
        {SET_SENSOR_SETTING,    TRUE,      FALSE,     set_sensor_setting  },
        {GET_SENSOR_SETTING,    FALSE,     FALSE,     get_sensor_setting  },
        {GET_SENSOR_VALUE,      FALSE,     FALSE,     get_sensor_value    },
//        {FIRMWARE_UPDATE,       FALSE,     FALSE,     fireware_update     },
        {GET_LOG,               FALSE,     FALSE,     get_log             },
        {CLEAR_LOG,             TRUE,      FALSE,     clear_log           },
        {ACK,                   FALSE,     FALSE,     receive_ack         },
        {DEV_DISTINGUISH,       TRUE,      FALSE,     dev_distinguish     },
        {DISCONNECT,            TRUE,      TRUE,      disconnect          },
		{IGNORE_BROADCAST,      FALSE,     FALSE,     ignore_broadcast    },
        {MAX_COMMAND,           FALSE,     FALSE,     NULL                }
};

static int generate_send_packet(struct frame *reply, struct frame *receive, uint16_t sequence, uint16_t type, const uint8_t *data, size_t size)
{
    if (size > 0 && data == NULL)
        return -1;

    reply->start[0]        = START_BYTE_1;
    reply->start[1]        = START_BYTE_2;
    reply->start[2]        = START_BYTE_3;
    reply->group           = hub.lora_info.group;
    reply->header.section  = receive->header.section;
    reply->header.sequence = sequence;
    reply->header.type     = type;
    reply->header.preserve = 0;

    memcpy(reply->header.dest_id, receive->header.sour_id, sizeof(reply->header.dest_id));
    memcpy(reply->header.sour_id, hub.lora_info.mac, sizeof(reply->header.sour_id));

    reply->header.payload_len = size;
    if (size > 0) {
        reply->header.payload_len += sizeof(struct payload);
        memcpy(reply->payload.data, data, size);

        reply->payload.checksum = 0;
        reply->payload.checksum = calculate_crc((uint8_t *)&reply->payload, reply->header.payload_len);
    }

    reply->header.checksum  = 0;
    reply->header.checksum  = calculate_crc((uint8_t *)&reply->header, sizeof(struct header));

    return size == 0 ? sizeof(struct frame) - sizeof(struct payload) : sizeof(struct frame) + size;
}

void send_reply(struct frame *frame, uint16_t sequence, uint16_t type, const uint8_t *data, uint32_t size)
{
    int i;
    int length;

    IArg key = GateHwi_enter(GateHwi_handle(&hub.send_gate));

    length = 0;
    for (i = 0; i < MAX_SEND_QUEUE; ++i) {
        if (! hub.send[i].flag) {
            length = generate_send_packet((struct frame *)hub.send[i].reply, frame, sequence, type, data, size);

            if (length > 0) {
                hub.send[i].flag         = TRUE;
                hub.send[i].countdown    = NO_RESPONSES_TIME;
                hub.send[i].reply_times  = WAIT_ACK_TIME - 1;
                hub.send[i].reply_length = length;
            }
            break;
        }
    }

    GateHwi_leave(GateHwi_handle(&hub.send_gate), key);

    if (i < MAX_SEND_QUEUE && length > 0) {
        write_lora((const void *)hub.send[i].reply, length, FALSE);

        if (type != SUCCESS && type != FAIL)
			send_message_data(data, size);
    }

    return;
}

void send_ack(struct frame *frame)
{
    int      i;
    int      length;
    uint8_t  reply[sizeof(struct frame)];


    length = generate_send_packet((struct frame *)reply, frame, frame->header.sequence, ACK, NULL, 0);
    if (length > 0) {
        for (i = 0; i < REPEAT_ACK; ++i) {
            write_lora((const void *)reply, length, FALSE);

            if (REPEAT_ACK > 1)
            	Task_sleep(ONE_SECOND / 10); // 100mS
        }
    }

    return;
}

void process_frame(UArg arg0, UArg arg1)
{
    int  result;
    struct command   *ptr;
    struct frame     *frame;

    frame = (struct frame *)arg1;

    if (calculate_crc((uint8_t *)&frame->payload, frame->header.payload_len) == 0) {
    	// Do not change LoRa module back to mode 3, Let timer function do it for save time if already start.
    	set_lora_mode(LORA_MODE_1);

        if (frame->header.type != ACK) {
            send_ack(frame);

			if (hub.first_start) {
				if (frame->header.type != SYSTEM_START && frame->header.type != DEV_DISTINGUISH && frame->header.type != IGNORE_BROADCAST) {
					result = SYSTEM_NOT_START;
					send_reply(frame, frame->header.sequence + 1, FAIL, (uint8_t *)&result, sizeof(int));

					goto Exit;
				}
			} else {
				if (frame->header.type == SYSTEM_START) {
					result = SYSTEM_ALREADY_START;
					send_reply(frame, frame->header.sequence + 1, FAIL, (uint8_t *)&result, sizeof(int));

					goto Exit;
				}
			}
        }

        send_message_value("Receive command", frame->header.type);

        ptr = commands;
        while (ptr->type != MAX_COMMAND) {
            if (frame->header.type == ptr->type && ptr->command_fxn != NULL) {
                result = ptr->command_fxn(frame);

                if (result == EXECUTE_SUCCESS && ptr->report)
                	send_reply(frame, frame->header.sequence + 1, SUCCESS, NULL, 0);

                if (result != EXECUTE_SUCCESS) {
					result |= ((int32_t)frame->header.type) << 24;
					send_reply(frame, frame->header.sequence + 1, FAIL, (uint8_t *)&result, sizeof(int));
                }

                send_message_value("Command result", result);

                if (result == EXECUTE_SUCCESS && ptr->reboot) {
                	set_lora_mode(LORA_MODE_4);
                    SysCtrlSystemReset();
                }

                break;
            }
            ++ptr;
        }

        if (ptr->type == MAX_COMMAND)
        	send_message_value("Bad command", frame->header.type);
    } else {
    	send_message("Payload CRC Error");
    }

Exit:

#if MUTIL_TASK
    {
        IArg key = GateHwi_enter(GateHwi_handle(&hub.lora_rec_gate));
        ((struct rec_space *)arg0)->flag = TERMINATED_FLAG;
        Semaphore_post(Semaphore_handle(&hub.task_reclaim_smp));
        GateHwi_leave(GateHwi_handle(&hub.lora_rec_gate), key);
    }
#endif

    if (! hub.first_start)
		set_lora_mode(LORA_MODE_3);

    return;
}
