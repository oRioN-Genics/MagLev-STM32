#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "pwm.h"
#include "adc.h"
#include "pid.h"
#include <stdint.h>

PID_Controller maglev_pid;

int main (void) {
    PWM_Init();
    ADC_Init();

    PID_Init(&maglev_pid, 5.0f, 0.0f, 1.0f, 2000.0f);

    while (1) {
        uint16_t sensor_value = ADC_Read();
        float pwm_out = PID_Compute(&maglev_pid, (float)sensor_value);
        PWM_SetDutyCycle((uint16_t)pwm_out);
    }
}