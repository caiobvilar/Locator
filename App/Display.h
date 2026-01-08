// App/Display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void
Display_Init(void);
void
Display_SetOrientation(uint32_t orientation);
void
Display_FillScreen(uint16_t color);
void
Display_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void
Display_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#endif // DISPLAY_H
