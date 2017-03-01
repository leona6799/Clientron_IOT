/*
 * receive_fxn.c
 *
 *  Created on: 2016年5月31日
 *      Author: 03565
 */

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include "sensor_hub.h"
#include "hal_trng_wrapper.h"

#if MUTIL_TASK
static int get_usable_space(void)
{
    int  i;

    IArg key = GateHwi_enter(GateHwi_handle(&hub.lora_rec_gate));

    for (i = 0; i < MAX_RECEIVE_BUFFER; ++i) {
        if (hub.rec_entity[i].flag == VACANT_FLAG) {
            hub.rec_entity[i].flag = OCCUPIED_FLAG;
            break;
        }
    }

    GateHwi_leave(GateHwi_handle(&hub.lora_rec_gate), key);

    return i < MAX_RECEIVE_BUFFER ? i : BUFFER_NOT_FOUND;
}
#endif

static uint32_t get_trng_number()
{
	uint32_t rand;

	Power_setDependency(PowerCC26XX_PERIPH_TRNG);
	rand = HalTRNG_GetTRNG();
	Power_releaseDependency(PowerCC26XX_PERIPH_TRNG);

	return rand;
}

#if 1
static uint8_t *search_valid_frame(uint8_t *start, uint8_t *end)
{
    uint32_t delay;
    uint8_t  broadcast[] = BROADCAST_MAC_ADDRESS;

    while(start < end) {  //end永遠比start大
        if (*start != START_BYTE_1 || *(start + 1) != START_BYTE_2 || *(start + 2) != START_BYTE_3) {
            ++start; //start = start + 1
            continue;
        }

        if (calculate_crc((uint8_t *)&((struct frame *)start)->header, sizeof(struct header)) != 0) {
        	send_message("Header CRC Error");
            ++start; //往前挪動記憶體位址(往前挪動1個byte)
            continue;
        }

        if (start + sizeof(struct frame) - sizeof(struct payload) + ((struct frame *)start)->header.payload_len > end)
            return NULL;

        if (memcmp(broadcast, ((struct frame *)start)->header.dest_id, sizeof(broadcast)) == 0 && hub.ignore_broadcast_timer <= 0) {
        //if (memcmp(broadcast, ((struct frame *)start)->header.dest_id, sizeof(broadcast)) == 0) {
            delay = get_trng_number() % 500;
            Task_sleep(delay * ONE_SECOND / 500);

            // Do not change LoRa module back to mode 3, Let time function do it for save time if already start.
            set_lora_mode(LORA_MODE_1);
            send_ack((struct frame *)start);

            if (! hub.first_start)
            	set_lora_mode(LORA_MODE_3);

            start += sizeof(struct frame) - sizeof(struct payload);
            continue;
        }

        if (memcmp(hub.lora_info.mac, ((struct frame *)start)->header.dest_id, sizeof(hub.lora_info.mac)) != 0) {
            ++start;
            continue;
        }

        if (! hub.first_start && memcmp(hub.edge_mac, ((struct frame *)start)->header.sour_id, sizeof(hub.edge_mac)) != 0) {
            ++start;
            continue;
        }

        break;
    }

    return start < end ? start : NULL;
}
#else
static uint8_t *read_uart(UART_Handle uart, uint8_t *buf, size_t length)
{
    Bool          find;
    uint8_t      *ptr;
    uint8_t      *end;
    int           size;
    uint8_t       broadcast[] = BROADCAST_MAC_ADDRESS;

    find = FALSE;
    while (! find) {
        size = UART_read(uart, (void *)buf, length);
        if (size == UART_ERROR)
            continue;

        ptr = buf;
        end = ptr + size - (sizeof(struct frame) - sizeof(struct payload));
        while (ptr < end) {
            if (*ptr != START_BYTE_1 || *(ptr + 1) != START_BYTE_2 || *(ptr + 2) != START_BYTE_3) {
                ++ptr;
                continue;
            }

            if (calculate_crc((uint8_t *)&((struct frame *)ptr)->header, sizeof(struct header)) != 0) {
                ++ptr;
                continue;
            }

            if (memcmp(broadcast, ((struct frame *)ptr)->header.dest_id, sizeof(broadcast)) == 0) {
                send_ack((struct frame *)ptr, 1);
                ++ptr;
                continue;
            }

            if (memcmp(hub.lora_info.mac, ((struct frame *)ptr)->header.dest_id, sizeof(hub.lora_info.mac)) != 0) {
                ++ptr;
                continue;
            }

            if (! hub.first_start && memcmp(hub.edge_mac, ((struct frame *)ptr)->header.sour_id, sizeof(hub.edge_mac)) != 0) {
                ++ptr;
                continue;
            }

            find = TRUE;
            break;
        }
    }

    return ptr;
}
#endif

static UART_Handle open_lora(uint32_t rate)
{
    UART_Params uartParams;
    UART_Handle uart;
    uint32_t    baud_rate[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600};

    if (rate >= MAX_UART_RATE)
        return NULL;

    UART_Params_init(&uartParams);
    uartParams.writeDataMode   = UART_DATA_BINARY;
    uartParams.readDataMode    = UART_DATA_BINARY;
    uartParams.readReturnMode  = UART_RETURN_FULL;
    uartParams.readMode        = UART_MODE_BLOCKING;
    uartParams.writeMode       = UART_MODE_BLOCKING;
    uartParams.readEcho        = UART_ECHO_OFF;
    uartParams.baudRate        = baud_rate[rate];
    uartParams.readTimeout     = 5 * (ONE_SECOND  / 10); // 500ms

    uart = UART_open(Board_UART, &uartParams);
    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    UART_control(uart, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);

    return uart;
}

void write_lora(const void *buffer, size_t size, Bool disable)
{
	//int        count;
	//bool       rx_not_empty;
	//uint32_t   delay;
    IArg       key;
    PIN_Config pin_config;

    key = GateHwi_enter(GateHwi_handle(&hub.lora_write_gate));

    if (disable) {
        pin_config = PIN_getConfig(LoRa_BZ);
        PIN_setConfig(intPinHandle, PIN_BM_IRQ, LoRa_BZ | PIN_IRQ_DIS);
    }

#if 0
    count = 0;
    while(count++ < 5) {
    	UART_control(hub.lora, UARTCC26XX_CMD_RX_FIFO_FLUSH, NULL);
    	Task_sleep(ONE_SECOND / 100);
    	UART_control(hub.lora, UART_CMD_ISAVAILABLE, &rx_not_empty);

    	if (rx_not_empty) {
    		//
    		// wait for 10ms ~ 100ms
    		//
			delay = get_trng_number() % 100;
			if (delay < 10) delay += 10;

			Task_sleep(delay * ONE_SECOND / 1000);
    	} else {
    		UART_write(hub.lora, buffer, size);
    		break;
    	}
    }
#else
    UART_write(hub.lora, buffer, size);
#endif

    if (disable)
        PIN_setConfig(intPinHandle, PIN_BM_IRQ, pin_config);

    GateHwi_leave(GateHwi_handle(&hub.lora_write_gate), key);
}

uint16_t calculate_crc(uint8_t *buf, size_t size)
{
    int       i;
    int       even_size;
    uint32_t  checksum;

    if (size == 0)
        return 0;

    checksum = 0;
    even_size = size & (~0x01);
    for (i = 0; i < even_size; i += (sizeof(uint16_t) / sizeof(uint8_t)))
        checksum += *(uint16_t *)&buf[i];

    if (size & 0x01 == 0x01)
        checksum += buf[i];

    while ((checksum >> 16) > 0)
        checksum = (checksum >> 16) + (checksum & 0x0000ffff);

    return (uint16_t)~checksum;
}

#if MUTIL_TASK
void reclaim_fxn(UArg arg0, UArg arg1)
{
    int  i;

    while (TRUE) {
        Semaphore_pend(Semaphore_handle(&hub.task_reclaim_smp), BIOS_WAIT_FOREVER);

        IArg key = GateHwi_enter(GateHwi_handle(&hub.lora_rec_gate));

        for (i = 0; i < MAX_RECEIVE_BUFFER; ++i) {
            if (hub.rec_entity[i].flag == TERMINATED_FLAG) {
                Task_destruct(&hub.rec_entity[i].task_Struct);
                hub.rec_entity[i].flag = VACANT_FLAG;
            }
        }

        Semaphore_reset(Semaphore_handle(&hub.task_reclaim_smp), 0);
        GateHwi_leave(GateHwi_handle(&hub.lora_rec_gate), key);
    }
}
#endif

void receive_fxn(UArg arg0, UArg arg1)
{
#if 0
    spi_clear_everything();
#else
    {
        int      i, rate;
        int      size;
        uint8_t  prefix[] = LORA_SETTING_PREFIX;
        uint8_t  rx_buf[LORA_BUFFER_LENGTH];
        uint8_t  tx_buf[LORA_BUFFER_LENGTH];

        struct config     config;
        struct lora_resp *resp;

        send_message("System Start");

        //
        // Detect LoRa module and LoRa's currently setting
        //
#if DEBUG_WITH_J_TAG
        hub.first_start = TRUE;
#else
        spi_get_config(&config);
        hub.first_start = (config.first_sign != NOT_FIRST_USE_SIGN);
#endif

        memcpy(tx_buf, prefix, sizeof(prefix));
        if (hub.first_start) {
            struct lora_config default_setting = LORA_DEFAULT_SETTING;

            rate = _9600BPS;
            if (LORA_WAKE_UP_TIME <= 0x0b)
                default_setting.wakeup_time = LORA_WAKE_UP_TIME;

            memcpy(&tx_buf[LORA_SETTING_PREFIX_LENGTH], &default_setting, sizeof(struct lora_config));
        } else {
            rate = config.lora.UART_rate;
            memcpy(&tx_buf[LORA_SETTING_PREFIX_LENGTH], &config.lora, sizeof(struct lora_config));
        }

        // Setting LoRa with default or previous configuration
        set_pin(Board_Red_LED, 1);
        set_lora_mode(LORA_MODE_4);

        resp = (struct lora_resp *)rx_buf;
        hub.lora = open_lora(_9600BPS);
        do {
            size = LORA_SETTING_PREFIX_LENGTH + sizeof(struct lora_config);
            UART_write(hub.lora, (void *)tx_buf, size);

            i = 0;
            while (++i < 10) {
                size = UART_read(hub.lora, (void *)rx_buf, sizeof(rx_buf));

                if (size != UART_ERROR && resp->prefix == LORA_RESPONSE_PREFIX && resp->suffix == LORA_RESPONSE_SUFFIX) {
                    memcpy(&hub.lora_info, &resp->info, sizeof(struct lora_info));
                    break;
                }
            }
        } while(i >= 10);
        UART_close(hub.lora);

        set_lora_mode(LORA_MODE_3);
        set_pin(Board_Red_LED, 0);

        // reopen UART with correct rate
        hub.lora = open_lora(rate);

        // Clear Rx buffer of UART
        UART_control(hub.lora, UARTCC26XX_CMD_RX_FIFO_FLUSH, NULL);

        if (! hub.first_start) {
        	set_pin(Board_Green_LED, 0);

        	hub.already_setting = config.already_setting;
            memcpy(hub.edge_mac, config.edge_mac, sizeof(hub.edge_mac));

            for (i = 0; i < MAX_SOCKET; ++i) {
                hub.socket[i].sensor_id    = config.sensor[i].sensor_id;
                hub.socket[i].bus_type     = config.sensor[i].bus_type;
                hub.socket[i].catch_period = config.sensor[i].catch_period;
                hub.socket[i].countdown    = config.sensor[i].catch_period * LOST_CONTACT_TIME;
            }


            Clock_start(clkHandle);
        } else {
        	if (config.already_setting) {
        		config.already_setting = FALSE;
        		spi_store_config(&config);
        	}
        }
    }

#if 0
    {
        int           id;
        uint8_t      *start;
        Task_Params   taskParams;
        uint8_t       rec_buf[MAX_ROLA_PACKAGE_LENGTH];

        Task_Params_init(&taskParams);
        taskParams.stackSize = TASKSTACKSIZE;
        while (TRUE) {
            start = read_uart(hub.lora, (void *)rec_buf, sizeof(rec_buf));

            id = get_usable_space();
            if (id != BUFFER_NOT_FOUND) {
                memcpy(hub.rec_entity[id].rec_buf, rec_buf, sizeof(hub.rec_entity[id].rec_buf));

                taskParams.stack = hub.rec_entity[id].rec_stack;
                taskParams.arg0  = (UArg)&hub.rec_entity[id];
                taskParams.arg1  = (UArg)hub.rec_entity[id].rec_buf + (start - rec_buf);

                Task_construct(&hub.rec_entity[id].task_Struct, (Task_FuncPtr)process_frame, &taskParams, NULL);
            }
        }
    }
#else

    {

        int           size;
        uint8_t      *ptr;
        uint8_t      *end;
        uint8_t       rec_buf[MAX_ROLA_PACKAGE_LENGTH];

#if MUTIL_TASK
        int           id;
        Task_Params   taskParams;
        Task_Params_init(&taskParams);
        taskParams.stackSize = TASKSTACKSIZE;
#endif

        memset(rec_buf, 0, sizeof(rec_buf));
        while (TRUE) {
            size = UART_read(hub.lora, (void *)rec_buf, sizeof(rec_buf));
            if (size <= 0) {
                UARTCC26XX_Object *object = hub.lora->object;

                if (size == 0 && object->status == UART_TIMED_OUT) {
                    //memset(rec_buf, 0, sizeof(rec_buf));
                	send_message("Wait for command");

                    PIN_setConfig((PIN_Handle)arg0, PIN_BM_IRQ, LoRa_BZ | PIN_IRQ_NEGEDGE);
                    Semaphore_pend(Semaphore_handle(&hub.lora_smp), BIOS_WAIT_FOREVER);
                    PIN_setConfig((PIN_Handle)arg0, PIN_BM_IRQ, LoRa_BZ | PIN_IRQ_DIS);
                }

                continue;
            }

            ptr = rec_buf;
            end = ptr + size;

            while(ptr != NULL) {
                ptr = search_valid_frame(ptr, end);
				if (ptr != NULL) {
#if MUTIL_TASK
					id = get_usable_space();
					if (id != BUFFER_NOT_FOUND) {
						memcpy(hub.rec_entity[id].rec_buf, ptr, size);

						taskParams.stack = hub.rec_entity[id].rec_stack;
						taskParams.arg0  = (UArg)&hub.rec_entity[id];
						taskParams.arg1  = (UArg)hub.rec_entity[id].rec_buf;

						Task_construct(&hub.rec_entity[id].task_Struct, (Task_FuncPtr)process_frame, &taskParams, NULL);
					}
#else
					process_frame((UArg)NULL, (UArg)ptr);
#endif
					ptr += sizeof(struct frame);
				}
            }
        }
    }
#endif
#endif
}
