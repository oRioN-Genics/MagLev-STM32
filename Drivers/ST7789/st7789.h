#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

void ST7789_Init(void);
void ST7789_WriteCommand(uint8_t cmd);
void ST7789_WriteData(uint8_t data);
void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_FillScreen(uint16_t color);
void ST7789_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color);
void ST7789_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t color, uint16_t bg_color);

#endif