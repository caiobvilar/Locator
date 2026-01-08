// App/Display.c
#include "Display.h"
#include "f411E_lcd.h"
#include "st7789.h"

static st7789_t*
display_handle(void)
{
    return F411E_LCD_GetHandle();
}

void
Display_Init(void)
{
    LCD_Init();
}

void
Display_SetOrientation(uint32_t orientation)
{
    LCD_SetOrientation(orientation);
}

void
Display_FillScreen(uint16_t color)
{
    st7789_t* lcd = display_handle();
    st7789_fill_rect(lcd, 0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void
Display_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    st7789_t* lcd = display_handle();
    st7789_fill_rect(lcd, x, y, 1, 1, color);
}

void
Display_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (!w || !h)
    {
        return;
    }
    st7789_t* lcd = display_handle();
    st7789_fill_rect(lcd, x, y, w, h, color);
}
