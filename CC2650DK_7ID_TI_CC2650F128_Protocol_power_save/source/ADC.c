/*
 * ADC.c
 *
 *  Created on: 2016/08/08
 *      Author: 03468
 */
#include <xdc/std.h>
#include <ti/drivers/ADC.h>

#include "sensor_hub.h"

void *open_adc(void)
{
	ADC_Handle   adc;
	ADC_Params   adcparams;

	ADC_Params_init(&adcparams);
	adc = ADC_open(Board_ADC1, &adcparams); //DIO_29

	return adc;
}

void close_adc (void *handle)
{
    ADC_close((ADC_Handle)handle);
}

Bool read_adc(void *handle, uint8_t address, void *data, size_t size)
{
#if 1
	uint16_t value;

	if (ADC_convert((ADC_Handle)handle, &value)  == ADC_STATUS_SUCCESS) {
		*(uint32_t *)data = ADC_convertRawToMicroVolts((ADC_Handle)handle, value);
		return TRUE;
	} else {
		return FALSE;
	}
#else
	return ADC_convert((ADC_Handle)handle, (uint16_t *)data) == ADC_STATUS_SUCCESS;
#endif
}

Bool write_adc(void *handle, uint8_t address, void *data, size_t size)
{
	return true;
}
