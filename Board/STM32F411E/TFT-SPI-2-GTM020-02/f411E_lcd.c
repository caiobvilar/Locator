#include "f411E_lcd.h"
#include "st7789.h"

/* Single driver instance */
static st7789_t lcd;

st7789_t
F411E_LCD_GetHandle(void)
{
    return lcd;
}

/* Prototype for the board port init we just created */
void
st7789_port_init_f411_spi(st7789_t* lcd);

void
LCD_Init(void)
{
    st7789_port_init_f411_spi(&lcd); /* fill callbacks + geometry */
    (void)st7789_init(&lcd);
    LCD_SetOrientation(LCD_ORIENTATION_PORTRAIT_ROT180);
}

void
LCD_SetOrientation(uint32_t orientation)
{
    st7789_orientation_t o;

    switch (orientation)
    {
    case LCD_ORIENTATION_PORTRAIT:
        o = ST7789_ORIENT_PORTRAIT;
        break;
    case LCD_ORIENTATION_LANDSCAPE:
        o = ST7789_ORIENT_LANDSCAPE;
        break;
    case LCD_ORIENTATION_PORTRAIT_ROT180:
        o = ST7789_ORIENT_PORTRAIT_INV;
        break;
    case LCD_ORIENTATION_LANDSCAPE_ROT180:
        o = ST7789_ORIENT_LANDSCAPE_INV;
        break;
    default:
        o = ST7789_ORIENT_PORTRAIT;
        break;
    }

    st7789_set_orientation(&lcd, o);
}
