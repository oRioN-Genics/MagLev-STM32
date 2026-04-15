#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "pwm.h"
#include <stdint.h>

int main (void) {
    PWM_Init();
    PWM_SetDutyCycle(uint16_t 500);

    while (1) {
        
    }
}