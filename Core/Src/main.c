#include "stm32f103xb.h"
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

int main (void) {
    RCC->APB2ENR |= (1 << 4);

    // this CRH has to be set to 0 first bcz they are not 0 originaly
    GPIOC->CRH &= ~(0xF << 20);
    // CRF = 00 and MODE = 10. hence, 0x2 = 0010. making this GPIO push pull o/p
    GPIOC->CRH |= (0x2 << 20);

    while (1) {
        GPIOC->ODR ^= (1 << 13); // toggle using XOR
        delay_ms(1000);
    }
}