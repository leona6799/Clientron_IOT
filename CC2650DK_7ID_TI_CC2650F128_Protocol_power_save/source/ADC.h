/*
 * adc.h
 *
 *  Created on: 2016/08/08
 *      Author: 03468
 */

#ifndef SOURCE_ADC_H_
#define SOURCE_ADC_H_

void *open_adc(void);
void close_adc(void *handle);

Bool read_adc(void *handle, uint8_t address, void *data, size_t size);
Bool write_adc(void *handle, uint8_t address, void *data, size_t size);

#endif /* SOURCE_ADC_H_ */
