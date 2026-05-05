#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "pwm.h"
#include "adc.h"
#include "pid.h"

// --- NEW DISPLAY HEADERS ---
#include "spi.h"
#include "st7789.h"
#include <stdint.h>
#include <stdio.h> // Required for snprintf

// Colors for the display
#define COLOR_BLACK 0x0000
#define COLOR_GREEN 0x07E0
#define COLOR_RED   0xF800
#define COLOR_WHITE 0xFFFF

PID_Controller maglev_pid;
volatile uint16_t tick_flag = 0;

void Timer_Init(void) {
    SysTick->LOAD = 8000 - 1; // 1ms tick at 8MHz
    SysTick->VAL = 0;
    SysTick->CTRL |= (0x7 << 0);
}

void SysTick_Handler(void) {
    tick_flag = 1;
}

int main (void) {
    // 1. Hardware Init
    PWM_Init();
    ADC_Init();
    
    // 2. Display Init
    SPI1_Init(); 
    ST7789_Init();
    ST7789_FillScreen(COLOR_BLACK);
    ST7789_DrawString(10, 10, "MAGLEV SYSTEM", COLOR_WHITE, COLOR_BLACK);

    // 3. System Init
    PID_Init(&maglev_pid, 1.0f, 0.0f, 1.0f, 2000.0f);
    Timer_Init();

    // Secondary counter for the display refresh rate
    uint16_t display_timer = 0; 
    char text_buffer[32]; 

    while (1) {
        if (tick_flag == 1) {
            tick_flag = 0;
            display_timer++; // Increment the slow loop counter
            
            // FAST LOOP: Runs every 1 millisecond (1kHz)
            uint16_t sensor_value = ADC_Read();
            float pwm_out = PID_Compute(&maglev_pid, (float)sensor_value);
            PWM_SetDutyCycle((uint16_t)pwm_out);

            // SLOW LOOP: Runs every 100 milliseconds (10Hz)
            if (display_timer >= 100) {
                display_timer = 0; // Reset counter

                // Print the raw ADC sensor value
                snprintf(text_buffer, sizeof(text_buffer), "Sens: %04d", sensor_value);
                ST7789_DrawString(10, 40, text_buffer, COLOR_GREEN, COLOR_BLACK);

                // Print the computed PWM output
                snprintf(text_buffer, sizeof(text_buffer), "PWM:  %04d", (uint16_t)pwm_out);
                ST7789_DrawString(10, 60, text_buffer, COLOR_RED, COLOR_BLACK);
            }
        }
    }
}