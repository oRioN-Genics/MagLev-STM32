#include "stm32f1xx.h"

int main (void) {
    RCC->APB2ENR |= (1 << 4);

    // this CRH has to be set to 0 first bcz they are not 0 originaly
    GPIOC->CRH &= ~(0xF << 20);
    // CRF = 00 and MODE = 10. hence, 0x2 = 0010. making this GPIO push pull o/p
    GPIOC->CRH |= (0x2 << 20);

    while (1) {
        GPIOC->ODR ^= (1 << 13); // toggle using XOR

        for (volatile uint32_t i = 0; i < 500000; i++) {
            // do nothing
        }
    }
}