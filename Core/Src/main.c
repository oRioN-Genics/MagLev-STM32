#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "pwm.h"
#include "adc.h"
#include "pid.h"
#include <stdint.h>
#include <stdio.h>

#include "spi.h"
#include "st7789.h"

#define COLOR_BLACK 0x0000
#define COLOR_GREEN 0x07E0
#define COLOR_RED   0xF800
#define COLOR_WHITE 0xFFFF
#define COLOR_CYAN  0x07FF 

PID_Controller maglev_pid;
volatile uint16_t sensor_value = 0;
volatile float pwm_out = 0;

volatile float live_kp = 29.9f;
volatile uint32_t ms_ticks = 0;
volatile uint32_t last_button_time = 0;
volatile uint32_t last_display_time = 0; // New timer for the display

void Timer_Init(void) {
    SysTick->LOAD = 8000 - 1; // 1ms tick
    SysTick->VAL = 0;
    SysTick->CTRL |= (0x7 << 0);
}

// ==========================================
// THE FAST LOOP (Hardware Interrupt)
// Runs EXACTLY every 1ms, pre-empting everything else
// ==========================================
void SysTick_Handler(void) {
    ms_ticks++; 
    
    // Update live variables
    maglev_pid.Kp = live_kp;
    
    // Mission Critical Control Math
    sensor_value = ADC_Read();
    pwm_out = PID_Compute(&maglev_pid, (float)sensor_value);
    PWM_SetDutyCycle((uint16_t)pwm_out);
}

void Button_Interrupt_Init(void) {
    RCC->APB2ENR |= (1 << 3) | (1 << 0); 
    GPIOB->CRL &= ~((0xF << 0) | (0xF << 4)); 
    GPIOB->CRL |= (0x8 << 0) | (0x8 << 4);    
    GPIOB->ODR |= (1 << 0) | (1 << 1);        

    AFIO->EXTICR[0] &= ~((0xF << 0) | (0xF << 4));
    AFIO->EXTICR[0] |= (0x1 << 0) | (0x1 << 4); 

    EXTI->IMR |= (1 << 0) | (1 << 1);
    EXTI->FTSR |= (1 << 0) | (1 << 1);

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
}

void EXTI0_IRQHandler(void) { 
    if (EXTI->PR & (1 << 0)) {
        EXTI->PR |= (1 << 0); 
        if (ms_ticks - last_button_time > 200) { 
            live_kp += 0.1f; 
            last_button_time = ms_ticks;
        }
    }
}

void EXTI1_IRQHandler(void) { 
    if (EXTI->PR & (1 << 1)) {
        EXTI->PR |= (1 << 1);
        if (ms_ticks - last_button_time > 200) {
            live_kp -= 0.1f;
            if (live_kp < 0.0f) live_kp = 0.0f; 
            last_button_time = ms_ticks;
        }
    }
}

int main (void) {
    PWM_Init();
    ADC_Init();
    Button_Interrupt_Init();

    SPI1_Init(); 
    ST7789_Init();
    ST7789_FillScreen(COLOR_BLACK);
    
    ST7789_DrawString(10, 10, "MAGLEV TUNING", COLOR_WHITE, COLOR_BLACK);
    ST7789_DrawString(10, 80, "Use PB0/PB1 to adjust", COLOR_WHITE, COLOR_BLACK);

    PID_Init(&maglev_pid, live_kp, 0.01f, 50.0f, 2350.0f);
    Timer_Init(); // This immediately turns on the SysTick Interrupt

    char text_buffer[32]; 

    // ==========================================
    // THE SLOW LOOP (Background Task)
    // ==========================================
    while (1) {
         if (ms_ticks - last_display_time >= 100) {
            last_display_time = ms_ticks; // Reset timer

            snprintf(text_buffer, sizeof(text_buffer), "Kp:   %5.1f", live_kp);
            ST7789_DrawString(10, 35, text_buffer, COLOR_CYAN, COLOR_BLACK);

            snprintf(text_buffer, sizeof(text_buffer), "Sens: %04d", sensor_value);
            ST7789_DrawString(10, 50, text_buffer, COLOR_GREEN, COLOR_BLACK);

            snprintf(text_buffer, sizeof(text_buffer), "PWM:  %04d", (uint16_t)pwm_out);
            ST7789_DrawString(10, 65, text_buffer, COLOR_RED, COLOR_BLACK);
        }
    }
}