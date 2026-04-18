#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "pwm.h"
#include "adc.h"
#include <stdint.h>

int main (void) {
    PWM_Init();
    ADC_Init();

    // PWM_SetDutyCycle(10);

    while (1) {
        uint16_t sensor_value = ADC_Read();
        uint16_t sensor_norm_val = ((uint32_t)sensor_value * 999) / 4095;
        // feed the normalized sensor reading to pwm
        PWM_SetDutyCycle(sensor_norm_val);
    }
}