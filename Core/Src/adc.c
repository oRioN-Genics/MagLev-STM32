#include "adc.h"
#include "stm32f1xx.h"
#include <stdint.h>

void delay_ms (uint32_t ms) {
    SysTick->LOAD = 8000 - 1;
    SysTick->VAL = 0; // set current to 0, forcing reset
    // set bit 2 (clock) to 1 for cpu clock and bit 0 to 1 for enable
    SysTick->CTRL = (1 << 2) | (1 << 0);

    for (uint32_t i = 0; i < ms; i++) {
        while ((SysTick->CTRL & (1 << 16)) == 0) {

        }
    }

    SysTick->CTRL = 0;
}

void ADC_Init(void) {
    // i/o port A clock enable
    RCC->APB2ENR |= (1 << 2);
    // enable clock for ADC1
    RCC->APB2ENR |= (1 << 9);

    // ADC1 is wired to PA1
    // no need to manually set CNF & MODE since it's value is 0000
    GPIOA->CRL &= ~(0xF << 4);

    // set channel 1 to 7.5 cycles
    ADC1->SMPR2 |= (0b001 << 3);
    // assign channel 1 as the 1st conversion in the sequence
    ADC1->SQR3 = 1;

    // A/D converter ON
    ADC1->CR2 |= (0x1 << 0);
    delay_ms(1);
    // calibrate
    ADC1->CR2 |= (0x1 << 2);

    while (ADC1->CR2 & 0x4) {

    }
}

uint16_t ADC_Read(void) {
    // if this bit holds 1, and a 1 is written to it, it starts the conversion
    ADC1->CR2 |= (0x1 << 0);

    // wait for the status register
    while ((ADC1->SR & (1 << 1)) == 0) {
        // we are trapped here until the hardware sets bit 1 to 1
    }

    // status register clears automatically when we read data register
    return ADC1->DR; 
}