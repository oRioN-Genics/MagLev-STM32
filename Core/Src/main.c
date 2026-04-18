#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "pwm.h"
#include "adc.h"
#include "pid.h"
#include <stdint.h>

PID_Controller maglev_pid;
volatile uint16_t tick_flag = 0;

void Timer_Init(void) {
    SysTick->LOAD = 8000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL |= (0x7 << 0);
}

void SysTick_Handler(void) {
    tick_flag = 1;
}

int main (void) {
    PWM_Init();
    ADC_Init();

    PID_Init(&maglev_pid, 1.0f, 0.0f, 1.0f, 2000.0f);
    Timer_Init();

    while (1) {
        if (tick_flag == 1) {
            tick_flag = 0;
            uint16_t sensor_value = ADC_Read();
            float pwm_out = PID_Compute(&maglev_pid, (float)sensor_value);
            PWM_SetDutyCycle((uint16_t)pwm_out);
        }
    }
}