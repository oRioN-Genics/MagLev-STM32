// #include "stm32f103xb.h"
// #include "stm32f1xx.h"
// #include "pwm.h"
// #include "adc.h"
// #include "pid.h"
// #include <stdint.h>

// PID_Controller maglev_pid;
// volatile uint16_t tick_flag = 0;
// volatile uint16_t sensor_value = 0;
// volatile float pwm_out = 0;

// void Timer_Init(void) {
//     SysTick->LOAD = 8000 - 1;
//     SysTick->VAL = 0;
//     SysTick->CTRL |= (0x7 << 0);
// }

// void SysTick_Handler(void) {
//     tick_flag = 1;
// }

// int main (void) {
//     PWM_Init();
//     ADC_Init();

//     PID_Init(&maglev_pid, 8.0f, 0.0f, 0.0f, 2220.0f);
//     Timer_Init();

//     while (1) {
//          if (tick_flag == 1) {
//             tick_flag = 0;
            
//             sensor_value = ADC_Read();
//             pwm_out = PID_Compute(&maglev_pid, (float)sensor_value);
//             PWM_SetDutyCycle((uint16_t)pwm_out);
//         }
//     }
// }


#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "pwm.h"
#include "adc.h"
#include "pid.h"
#include <stdint.h>

PID_Controller maglev_pid;
volatile uint16_t tick_flag = 0;
volatile uint16_t sensor_value = 0;
volatile float pwm_out = 0;

volatile float live_kp = 29.9f;
volatile uint32_t ms_ticks = 0;
volatile uint32_t last_button_time = 0;

void Timer_Init(void) {
    SysTick->LOAD = 8000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL |= (0x7 << 0);
}

void SysTick_Handler(void) {
    tick_flag = 1;
    ms_ticks++; 
}

void Button_Interrupt_Init(void) {
    RCC->APB2ENR |= (1 << 3) | (1 << 0); 

    GPIOB->CRL &= ~((0xF << 0) | (0xF << 4)); // clear PB0 and PB1 configs
    GPIOB->CRL |= (0x8 << 0) | (0x8 << 4);    // set to i/p with Pull (1000 = 0x8)
    GPIOB->ODR |= (1 << 0) | (1 << 1);        // set ODR high to activate Pull-Up

    // map EXTI0 to PB0 and EXTI1 to PB1
    AFIO->EXTICR[0] &= ~((0xF << 0) | (0xF << 4));
    AFIO->EXTICR[0] |= (0x1 << 0) | (0x1 << 4); // 0001 assigns it to Port B

    // unmask EXTI0 and EXTI1 to allow them to trigger
    EXTI->IMR |= (1 << 0) | (1 << 1);

    // set triggers to Falling Edge (Triggers when button connects to GND)
    EXTI->FTSR |= (1 << 0) | (1 << 1);

    // enable the Interrupts in the NVIC (Processor Core)
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
}

void EXTI0_IRQHandler(void) { // PB0 (Increase)
    if (EXTI->PR & (1 << 0)) {
        EXTI->PR |= (1 << 0); // clear the pending flag immediately
        
        // 200ms debounce check
        if (ms_ticks - last_button_time > 200) { 
            live_kp += 0.1f; // the impactful step size
            last_button_time = ms_ticks;
        }
    }
}

void EXTI1_IRQHandler(void) { // PB1 (Decrease)
    if (EXTI->PR & (1 << 1)) {
        EXTI->PR |= (1 << 1);
        
        if (ms_ticks - last_button_time > 200) {
            live_kp -= 0.1f;
            if (live_kp < 0.0f) live_kp = 0.0f; // safety clamp
            last_button_time = ms_ticks;
        }
    }
}

int main (void) {
    PWM_Init();
    ADC_Init();
    Button_Interrupt_Init(); // boot up the buttons

    PID_Init(&maglev_pid, live_kp, 0.005f, 50.0f, 2350.0f);
    Timer_Init();

    while (1) {
         if (tick_flag == 1) {
            tick_flag = 0;
            
            // overwrite the PID struct with the live tuning variable
            maglev_pid.Kp = live_kp;
            sensor_value = ADC_Read();
            pwm_out = PID_Compute(&maglev_pid, (float)sensor_value);
            PWM_SetDutyCycle((uint16_t)pwm_out);
        }
    }
}