// App/Display.h
#pragma once
#include <stdint.h>

// RGB color macros for convenience (converts to RGB565)
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))
#define COLOR_RED RGB565(255, 0, 0)
#define COLOR_GREEN RGB565(0, 255, 0)
#define COLOR_BLUE RGB565(0, 0, 255)
#define COLOR_WHITE RGB565(255, 255, 255)
#define COLOR_BLACK RGB565(0, 0, 0)
#define COLOR_YELLOW RGB565(255, 255, 0)
#define COLOR_CYAN RGB565(0, 255, 255)
#define COLOR_MAGENTA RGB565(255, 0, 255)

void
Display_Init(void);
void
Display_SetOrientation(uint32_t orientation);
void
Display_FillScreenRGB(uint8_t r, uint8_t g, uint8_t b);
void
Display_DrawPixelRGB(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);
void
Display_FillRectRGB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b);
void
Display_DrawLineRGB(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b);
void
Display_DrawLineThickRGB(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b,
                         uint16_t thickness);
void
Display_DrawRulersRGB(uint8_t r, uint8_t g, uint8_t b, uint16_t thickness);
