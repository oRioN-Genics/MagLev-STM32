#include "st7789.h"
#include "spi.h"
#include "fonts.h"
#include "stm32f1xx.h"

// Quick hardware macros for readability
#define CS_LOW()   (GPIOA->BRR = (1 << 4))
#define CS_HIGH()  (GPIOA->BSRR = (1 << 4))
#define DC_CMD()   (GPIOA->BRR = (1 << 3))
#define DC_DATA()  (GPIOA->BSRR = (1 << 3))
#define RES_LOW()  (GPIOA->BRR = (1 << 2))
#define RES_HIGH() (GPIOA->BSRR = (1 << 2))

// Dummy delay for hardware startup
static void delay_stupid(uint32_t count) {
    while(count--) { __asm("nop"); }
}

void ST7789_WriteCommand(uint8_t cmd) {
    CS_LOW();
    DC_CMD();
    SPI_TransmitByte(cmd);
    CS_HIGH();
}

void ST7789_WriteData(uint8_t data) {
    CS_LOW();
    DC_DATA();
    SPI_TransmitByte(data);
    CS_HIGH();
}

void ST7789_Init(void) {
    // Hardware Reset
    RES_HIGH(); delay_stupid(50000);
    RES_LOW();  delay_stupid(50000);
    RES_HIGH(); delay_stupid(50000);

    // Initialization Sequence
    ST7789_WriteCommand(0x11); // Sleep Out
    delay_stupid(100000);

    ST7789_WriteCommand(0x3A); // Color Mode
    ST7789_WriteData(0x55);    // 16-bit RGB565

    ST7789_WriteCommand(0x36); // Memory Data Access Control (Orientation)
    ST7789_WriteData(0x00);    

    ST7789_WriteCommand(0x21); // Display Inversion ON (Standard for IPS ST7789)
    ST7789_WriteCommand(0x13); // Normal Display Mode ON
    
    ST7789_WriteCommand(0x29); // Display ON
    delay_stupid(50000);
}

void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    ST7789_WriteCommand(0x2A); // Column Address Set
    ST7789_WriteData(x0 >> 8); ST7789_WriteData(x0 & 0xFF);
    ST7789_WriteData(x1 >> 8); ST7789_WriteData(x1 & 0xFF);

    ST7789_WriteCommand(0x2B); // Row Address Set
    ST7789_WriteData(y0 >> 8); ST7789_WriteData(y0 & 0xFF);
    ST7789_WriteData(y1 >> 8); ST7789_WriteData(y1 & 0xFF);

    ST7789_WriteCommand(0x2C); // Memory Write
}

void ST7789_FillScreen(uint16_t color) {
    // Assuming a 240x240 screen, change bounds if yours is 240x320
    ST7789_SetWindow(0, 0, 239, 239); 
    
    uint8_t color_hi = color >> 8;
    uint8_t color_lo = color & 0xFF;

    CS_LOW();
    DC_DATA();
    for (uint32_t i = 0; i < 240 * 240; i++) {
        SPI_TransmitByte(color_hi);
        SPI_TransmitByte(color_lo);
    }
    CS_HIGH();
}

void ST7789_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color) {
    // We only have ASCII 32 to 90 in our slim font file
    if (c < 32 || c > 90) return;
    
    uint8_t index = c - 32;
    ST7789_SetWindow(x, y, x + 4, y + 7);
    
    uint8_t c_hi = color >> 8;
    uint8_t c_lo = color & 0xFF;
    uint8_t bg_hi = bg_color >> 8;
    uint8_t bg_lo = bg_color & 0xFF;

    CS_LOW();
    DC_DATA();
    
    for (int col = 0; col < 5; col++) {
        uint8_t line = Font_5x7[index][col];
        for (int row = 0; row < 8; row++) {
            if (line & 0x01) {
                SPI_TransmitByte(c_hi);
                SPI_TransmitByte(c_lo);
            } else {
                SPI_TransmitByte(bg_hi);
                SPI_TransmitByte(bg_lo);
            }
            line >>= 1;
        }
    }
    CS_HIGH();
}

void ST7789_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg_color) {
    uint16_t current_x = x;
    while (*str) {
        ST7789_DrawChar(current_x, y, *str, color, bg_color);
        current_x += 6; // Move right by 6 pixels for the next character
        str++;
    }
}