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

    uint16_t w = LCD_WIDTH;
    uint16_t h = LCD_HEIGHT;

    // TEMP: adapt to current orientation
    if (lcd->orientation == ST7789_ORIENT_LANDSCAPE || lcd->orientation == ST7789_ORIENT_LANDSCAPE_INV)
    {
        uint16_t tmp = w;
        w = h;
        h = tmp;
    }

    st7789_fill_rect(lcd, 0, 0, w, h, color);
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
