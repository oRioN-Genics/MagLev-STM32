#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "pwm.h"
#include "adc.h"
#include "pid.h"
#include <stdint.h>
#include <stdio.h> // Required for snprintf

#include "spi.h"
#include "st7789.h"

#define COLOR_BLACK 0x0000
#define COLOR_GREEN 0x07E0
#define COLOR_RED   0xF800
#define COLOR_WHITE 0xFFFF
#define COLOR_CYAN  0x07FF 

PID_Controller maglev_pid;
volatile uint16_t tick_flag = 0;
volatile uint16_t sensor_value = 0;
volatile float pwm_out = 0;

volatile float live_kp = 29.9f;
volatile uint32_t ms_ticks = 0;
volatile uint32_t last_button_time = 0;

void Timer_Init(void) {
    SysTick->LOAD = 8000 - 1; // 1ms tick
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

    EXTI->IMR |= (1 << 0) | (1 << 1);
    EXTI->FTSR |= (1 << 0) | (1 << 1);

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
}

void EXTI0_IRQHandler(void) { // PB0 (Increase)
    if (EXTI->PR & (1 << 0)) {
        EXTI->PR |= (1 << 0); 
        
        if (ms_ticks - last_button_time > 200) { 
            live_kp += 0.1f; 
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
    // 1. Hardware Init
    PWM_Init();
    ADC_Init();
    Button_Interrupt_Init();

    // 2. Display Init
    SPI1_Init(); 
    ST7789_Init();
    ST7789_FillScreen(COLOR_BLACK);
    
    // Draw static headers that never need to update
    ST7789_DrawString(10, 10, "MAGLEV TUNING", COLOR_WHITE, COLOR_BLACK);
    ST7789_DrawString(10, 80, "Use PB0/PB1 to adjust", COLOR_WHITE, COLOR_BLACK);

    // 3. System Init
    PID_Init(&maglev_pid, live_kp, 0.005f, 50.0f, 2350.0f);
    Timer_Init();

    uint16_t display_timer = 0; 
    char text_buffer[32]; 

    while (1) {
         if (tick_flag == 1) {
            tick_flag = 0;
            display_timer++; // Advance the slow loop counter
            
            // ==========================================
            // FAST LOOP: 1kHz (Runs every 1ms)
            // ==========================================
            maglev_pid.Kp = live_kp;
            sensor_value = ADC_Read();
            pwm_out = PID_Compute(&maglev_pid, (float)sensor_value);
            PWM_SetDutyCycle((uint16_t)pwm_out);

            // ==========================================
            // SLOW LOOP: 10Hz (Runs every 100ms)
            // ==========================================
            if (display_timer >= 100) {
                display_timer = 0; // Reset counter

                // Print the live Kp tuning value (Cyan color)
                // %5.1f ensures it shows 1 decimal place (e.g., " 29.9")
                snprintf(text_buffer, sizeof(text_buffer), "Kp:   %5.1f", live_kp);
                ST7789_DrawString(10, 35, text_buffer, COLOR_CYAN, COLOR_BLACK);

                // Print the raw ADC sensor value (Green)
                snprintf(text_buffer, sizeof(text_buffer), "Sens: %04d", sensor_value);
                ST7789_DrawString(10, 50, text_buffer, COLOR_GREEN, COLOR_BLACK);

                // Print the computed PWM output (Red)
                snprintf(text_buffer, sizeof(text_buffer), "PWM:  %04d", (uint16_t)pwm_out);
                ST7789_DrawString(10, 65, text_buffer, COLOR_RED, COLOR_BLACK);
            }
        }
    }
}