// f411E_lcd.c
#include "f411E_lcd.h"
#include "st7789.h"

static st7789_t lcd;

st7789_t*
F411E_LCD_GetHandle(void)
{
    return &lcd;
}

extern SPI_HandleTypeDef hspi1;
extern st7789_t lcd; // or keep lcd static above and forward-declare

static void
LCD_SetAddressWindow_raw(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t buf[4];

    // CASET (0x2A)
    buf[0] = x0 >> 8;
    buf[1] = x0 & 0xFF;
    buf[2] = x1 >> 8;
    buf[3] = x1 & 0xFF;
    lcd.write_cmdN(0x2A, buf, 4);

    // RASET (0x2B)
    buf[0] = y0 >> 8;
    buf[1] = y0 & 0xFF;
    buf[2] = y1 >> 8;
    buf[3] = y1 & 0xFF;
    lcd.write_cmdN(0x2B, buf, 4);

    // RAMWR (0x2C)
    lcd.write_cmd(0x2C);
}

static void
LCD_FillScreen_raw(uint16_t color)
{
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    LCD_SetAddressWindow_raw(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    LCD_CS_LOW();
    LCD_DC_HIGH();
    for (uint32_t i = 0; i < (uint32_t)LCD_WIDTH * LCD_HEIGHT; i++)
    {
        HAL_SPI_Transmit(&hspi1, &hi, 1, HAL_MAX_DELAY);
        HAL_SPI_Transmit(&hspi1, &lo, 1, HAL_MAX_DELAY);
    }
    LCD_CS_HIGH();
}

void
LCD_Init(void)
{
    st7789_port_init_spi(&lcd);
    (void)st7789_init(&lcd); // your copied init sequence

    LCD_FillScreen_raw(0xFFFF); // use old raw fill
    while (1)
    {
    }
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
