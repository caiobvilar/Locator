// f411E_lcd.c
#include "f411E_lcd.h"
#include "st7789.h"

static st7789_t lcd;

st7789_t*
F411E_LCD_GetHandle(void)
{
    return &lcd;
}

void
LCD_Init(void)
{
    st7789_port_init_spi(&lcd);

    bool ok = st7789_init(&lcd);
    if (!ok)
    {
        while (1)
        {
        } // callbacks wrong -> stop here
    }

    // Set the orientation you are actually using in the app
    LCD_SetOrientation(LCD_ORIENTATION_LANDSCAPE);

    // One single full-screen white fill
    st7789_fill_rect(&lcd, 0, 0, LCD_WIDTH, LCD_HEIGHT, 0xFFFF);

    while (1)
    {
    } // STOP here for now
}

void
LCD_SetOrientation(uint32_t orientation)
{
    st7789_orientation_t orient;

    switch (orientation)
    {
    case LCD_ORIENTATION_PORTRAIT:
        orient = ST7789_ORIENT_PORTRAIT;
        break;
    case LCD_ORIENTATION_LANDSCAPE:
        orient = ST7789_ORIENT_LANDSCAPE;
        break;
    case LCD_ORIENTATION_PORTRAIT_ROT180:
        orient = ST7789_ORIENT_PORTRAIT_INV;
        break;
    case LCD_ORIENTATION_LANDSCAPE_ROT180:
        orient = ST7789_ORIENT_LANDSCAPE_INV;
        break;
    default:
        orient = ST7789_ORIENT_PORTRAIT;
        break;
    }

    st7789_set_orientation(&lcd, orient);
}
