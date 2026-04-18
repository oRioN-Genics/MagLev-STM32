#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void delay_ms (uint32_t ms);

void ADC_Init(void);

uint16_t ADC_Read(void);

#endif