/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */
/* XDCtools Header files */
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/gates/GateMutex.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

/* TI-RTOS Header files */
#include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/ADC.h>
// #include <ti/drivers/Watchdog.h>

#include "source/sensor_hub.h"

/* Board Heap*/
Clock_Handle clkHandle;

Task_Struct  blinkStruct;
Char blinkStack[BLINK_TASKSTACKSIZE];

Task_Struct  timerStruct;
Char timerStack[TIME_TASKSTACKSIZE];

Task_Struct  receiveStruct;
Char receiveStack[MAIN_TASKSTACKSIZE];

#if MUTIL_TASK
Task_Struct  reclaimStruct;
Char reclaimStack[MAIN_TASKSTACKSIZE];
#endif

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State  ledPinState;

PIN_Handle intPinHandle;
static PIN_State  intPinState;


/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
    Board_Green_LED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_Red_LED   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    LoRa_P1         | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    LoRa_P2         | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    T6700_CO2       | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

PIN_Config interrupt_gpio[] = {
    LoRa_BZ       | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

// hub code start

struct sensor_hub hub;

#if SEND_DEBUG_MESSAGE_TO_UART || SEND_DEBUG_MESSAGE_TO_SPI
void send_message_c(char C)
{
#if SEND_DEBUG_MESSAGE_TO_UART
    struct frame  frame;

    memset((void *)&frame, C, sizeof(struct frame) - sizeof(struct payload));
    frame.start[0]        = START_BYTE_1;
    frame.start[1]        = START_BYTE_2;
    frame.start[2]        = START_BYTE_3;

    IArg uart_key = GateHwi_enter(GateHwi_handle(&hub.lora_write_gate));
    UART_write(hub.lora, (const void *)&frame, sizeof(struct frame) - sizeof(struct payload));
    GateHwi_leave(GateHwi_handle(&hub.lora_write_gate), uart_key);
#endif

#if SEND_DEBUG_MESSAGE_TO_SPI
	store_log((const uint8_t *)&C, 1);
#endif
}

void send_message(char *prefix)
{
#if SEND_DEBUG_MESSAGE_TO_UART
    IArg uart_key = GateHwi_enter(GateHwi_handle(&hub.lora_write_gate));
    UART_write(hub.lora, (const void *)prefix, strlen(prefix));
    GateHwi_leave(GateHwi_handle(&hub.lora_write_gate), uart_key);
#endif

#if SEND_DEBUG_MESSAGE_TO_SPI
    store_log((const uint8_t *)prefix, strlen(prefix));
#endif
}

void send_message_data(char *data, size_t size)
{
#if SEND_DEBUG_MESSAGE_TO_UART
    IArg uart_key = GateHwi_enter(GateHwi_handle(&hub.lora_write_gate));
    UART_write(hub.lora, (const void *)prefix, strlen(prefix));
    GateHwi_leave(GateHwi_handle(&hub.lora_write_gate), uart_key);
#endif

#if SEND_DEBUG_MESSAGE_TO_SPI
    store_log((const uint8_t *)data, size);
#endif
}

void send_message_value(char *prefix, uint32_t value)
{
#if SEND_DEBUG_MESSAGE_TO_UART
    char buffer[40];

    System_snprintf(buffer, sizeof(buffer), "%s : %08x\n", prefix, value);
    IArg uart_key = GateHwi_enter(GateHwi_handle(&hub.lora_write_gate));
    UART_write(hub.lora, (const void *)buffer, strlen(buffer));
    GateHwi_leave(GateHwi_handle(&hub.lora_write_gate), uart_key);
#endif

#if SEND_DEBUG_MESSAGE_TO_SPI
    char     log_buf[64];
	uint32_t log_size;

	log_size = System_snprintf(log_buf, sizeof(log_buf), "%s : %08x", prefix, value);
	store_log((const uint8_t *)log_buf, log_size);
#endif
}

void send_message_only_value(uint32_t value)
{
#if SEND_DEBUG_MESSAGE_TO_UART
    char buffer[10];

    System_snprintf(buffer, sizeof(buffer), "%08x\n", value);
    IArg uart_key = GateHwi_enter(GateHwi_handle(&hub.lora_write_gate));
    UART_write(hub.lora, (const void *)buffer, strlen(buffer));
    GateHwi_leave(GateHwi_handle(&hub.lora_write_gate), uart_key);
#endif

#if SEND_DEBUG_MESSAGE_TO_SPI
    char     log_buf[64];
	uint32_t log_size;

	log_size = System_snprintf(log_buf, sizeof(log_buf), "%08x", value);
	store_log((const uint8_t *)log_buf, log_size);
#endif
}
#endif

static void initialization(void)
{
    int i;

    hub.blink  = FALSE;
    hub.ignore_broadcast_timer = 0;

    GateHwi_construct(&hub.socket_gate, NULL);
    for (i = 0; i < MAX_SOCKET; ++i) {
        hub.socket[i].sensor_id    = NOT_CONNECT;
        hub.socket[i].bus_type     = MAX_BUS_TYPE;
        hub.socket[i].catch_period = NONE_PERIOD;
        hub.socket[i].countdown    = 0;
    }

    Semaphore_construct(&hub.timer_smp, 0, NULL);
    Semaphore_construct(&hub.blink_smp, 0, NULL);
    Semaphore_construct(&hub.lora_smp,  0, NULL);

    GateHwi_construct(&hub.lora_write_gate, NULL);
    for (i = 0; i < MAX_BUS_TYPE; ++i)
        GateHwi_construct(&hub.bus_gate[i], NULL);

    GateHwi_construct(&hub.send_gate, NULL);
    for (i = 0; i < MAX_SEND_QUEUE; ++i)
        hub.send[i].flag = FALSE;

#if MUTIL_TASK
    Semaphore_construct(&hub.task_reclaim_smp, 0, NULL);
    GateHwi_construct(&hub.lora_rec_gate, NULL);

    for (i = 0; i < MAX_RECEIVE_BUFFER; ++i)
        hub.rec_entity[i].flag = VACANT_FLAG;
#endif

    return;
}

static void create_task(void)
{
    Task_Params  taskParams;
    Clock_Params clkParams;

    Task_Params_init(&taskParams);
    taskParams.stackSize = MAIN_TASKSTACKSIZE;
    taskParams.stack = receiveStack;
    taskParams.arg0  = (xdc_UArg)intPinHandle;
    Task_construct(&receiveStruct, (Task_FuncPtr)receive_fxn, &taskParams, NULL);

#if MUTIL_TASK
    Task_Params_init(&taskParams);
    taskParams.stackSize = MAIN_TASKSTACKSIZE;
    taskParams.stack = reclaimStack;
    Task_construct(&reclaimStruct, (Task_FuncPtr)reclaim_fxn, &taskParams, NULL);
#endif

    Task_Params_init(&taskParams);
    taskParams.stackSize = BLINK_TASKSTACKSIZE;
    taskParams.stack = blinkStack;
    //taskParams.arg0  = (xdc_UArg)intPinHandle;
    Task_construct(&blinkStruct, (Task_FuncPtr)blink_task, &taskParams, NULL);

    Task_Params_init(&taskParams);
    taskParams.stackSize = TIME_TASKSTACKSIZE;
    taskParams.stack = timerStack;
    taskParams.arg0  = (xdc_UArg)intPinHandle;
    Task_construct(&timerStruct, (Task_FuncPtr)timer_task, &taskParams, NULL);

    Clock_Params_init(&clkParams);
    clkParams.period = ONE_SECOND;
    clkParams.startFlag = FALSE;
    clkHandle = Clock_create((Clock_FuncPtr)timer_fxn, ONE_SECOND, &clkParams, NULL);
}

inline uint_t get_pin(PIN_Id pinId)
{
    return PIN_getOutputValue(pinId);
}

inline void set_pin(PIN_Id pinId, uint_t val)
{
    PIN_setOutputValue(ledPinHandle, pinId, val);
}

#if 0
static Power_NotifyObj pNotifyObj;

static int PostNotify(unsigned int eventType, uintptr_t eventArg, uintptr_t clientArg)
{
    switch (eventType) {
    case PowerCC26XX_ENTERING_STANDBY:
        set_pin(Board_Red_LED, 0);
        //set_pin(Board_Green_LED, 0);
        break;
    case PowerCC26XX_AWAKE_STANDBY:
        set_pin(Board_Red_LED, 1);
        //set_pin(Board_Green_LED, 1);
        break;
    case PowerCC26XX_AWAKE_STANDBY_LATE:
        set_pin(Board_Red_LED, 1);
        //set_pin(Board_Green_LED, 1);
        break;
    default:
        break;
    }

    return Power_NOTIFYDONE;
}
#endif

static void start_receive(PIN_Handle handle, PIN_Id pinId)
{
    Semaphore_post(Semaphore_handle(&hub.lora_smp));
}

/*
 *  ======== main ========
 */
int main(void)
{
    /* Call board init functions */
    Board_initGeneral();
    Board_initI2C();

#if !DEBUG_WITH_J_TAG
    Board_initSPI();
#endif

    Board_initUART();
    Board_initADC();
    // Board_initWatchdog();

    initialization();

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if(ledPinHandle == NULL)
        System_abort("Error initializing board LED pins\n");

    intPinHandle = PIN_open(&intPinState, interrupt_gpio);
    if(intPinHandle == NULL)
        System_abort("Error initializing interrupt pins\n");

    PIN_registerIntCb(intPinHandle, start_receive);
    //Power_registerNotify(&pNotifyObj, PowerCC26XX_ENTERING_STANDBY | PowerCC26XX_AWAKE_STANDBY | PowerCC26XX_AWAKE_STANDBY_LATE, (Power_NotifyFxn)PostNotify, NULL);

    set_pin(Board_Green_LED, 1);
    create_task();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
