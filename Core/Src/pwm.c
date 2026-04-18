#include "pwm.h"
#include "stm32f1xx.h"

void PWM_Init(void) {
    // i/o port A clock enable
    RCC->APB2ENR |= (1 << 2);
    // timer 2 clock enable
    RCC->APB1ENR |= 1;

    // tim2-ch1 is wired to PA0 
    GPIOA->CRL &= ~(0xF << 0);
    // CNF = 10 for alt-func o/p push-pull and MODE = 11 for o/p 50MHz
    GPIOA->CRL |= (0xB << 0); // 1011 = 0xB

    // final freq = cpu clock / (PSC + 1) * (ARR + 1)
    // cpu clock = 8Mhz
    TIM2->PSC = 7;
    TIM2->ARR = 999; // now final freq is 1khz

    // activate PWM mode 1 
    TIM2->CCMR1 |= (0x6 << 4);
    // wait for the current cycle to finish if in case of change in duty cycle
    TIM2->CCMR1 |= (0x1 << 3);
    // capture/compare 1 o/p enable
    TIM2->CCER |= (0x1 << 0);
    TIM2->CR1 |= (0x1 << 0);

    TIM2->CCR1 = 0; // intialize with 0% power for safety
}

void PWM_SetDutyCycle(uint16_t duty) {
    if (duty > 999) {
        duty = 999;
    }

    // write directly to capture/compare register
    TIM2->CCR1 = duty;
}