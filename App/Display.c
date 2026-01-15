// App/Display.c

#include "Display.h"
#include "f411E_lcd.h"
#include "st7789.h"

static st7789_t*
handle(void)
{
    return F411E_LCD_GetHandle();
}

uint16_t
Display_GetWidth(void)
{
    st7789_t* lcd = handle();
    return lcd->width;
}

uint16_t
Display_GetHeight(void)
{
    st7789_t* lcd = handle();
    return lcd->height;
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
Display_FillScreenRGB(uint8_t r, uint8_t g, uint8_t b)
{
    st7789_t* lcd = handle();
    st7789_fill_rect_rgb(lcd, 0, 0, lcd->width, lcd->height, r, g, b);
}

void
Display_DrawPixelRGB(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b)
{
    st7789_t* lcd = handle();
    st7789_fill_rect_rgb(lcd, x, y, 1, 1, r, g, b);
}

void
Display_FillRectRGB(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b)
{
    if (!w || !h)
        return;

    st7789_t* lcd = handle();
    st7789_fill_rect_rgb(lcd, x, y, w, h, r, g, b);
}

void
Display_DrawLineRGB(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b)
{
    int16_t dx = (x1 > x0) ? (int16_t)(x1 - x0) : (int16_t)(x0 - x1);
    int16_t dy = (y1 > y0) ? (int16_t)(y1 - y0) : (int16_t)(y0 - y1);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    int16_t x = (int16_t)x0;
    int16_t y = (int16_t)y0;

    for (;;)
    {
        Display_DrawPixelRGB((uint16_t)x, (uint16_t)y, r, g, b);
        if (x == (int16_t)x1 && y == (int16_t)y1)
            break;

        int16_t e2 = (int16_t)(2 * err);
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

void
Display_DrawLineThickRGB(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b,
                         uint16_t thickness)
{
    if (thickness == 0)
        return;

    // Center line
    Display_DrawLineRGB(x0, y0, x1, y1, r, g, b);

    int16_t dx = (x1 > x0) ? (int16_t)(x1 - x0) : (int16_t)(x0 - x1);
    int16_t dy = (y1 > y0) ? (int16_t)(y1 - y0) : (int16_t)(y0 - y1);
    uint16_t half = (uint16_t)(thickness / 2);

    if (dx > dy)
    {
        // More horizontal – parallel lines above/below
        for (uint16_t o = 1; o <= half; ++o)
        {
            if (y0 >= o)
                Display_DrawLineRGB(x0, y0 - o, x1, y1 - o, r, g, b);
            if (y0 + o < Display_GetHeight())
                Display_DrawLineRGB(x0, y0 + o, x1, y1 + o, r, g, b);
        }
    }
    else
    {
        // More vertical – parallel lines left/right
        for (uint16_t o = 1; o <= half; ++o)
        {
            if (x0 >= o)
                Display_DrawLineRGB(x0 - o, y0, x1 - o, y1, r, g, b);
            if (x0 + o < Display_GetWidth())
                Display_DrawLineRGB(x0 + o, y0, x1 + o, y1, r, g, b);
        }
    }
}

void
Display_DrawRulersRGB(uint8_t r, uint8_t g, uint8_t b, uint16_t thickness)
{
    if (thickness == 0)
        return;

    // Horizontal ruler along top edge
    Display_DrawLineThickRGB(0, 0, Display_GetWidth() - 1, 0, r, g, b, thickness);

    // Major ticks every 10 px, 10 px tall
    for (uint16_t x = 0; x < Display_GetWidth(); x += 10)
        Display_DrawLineRGB(x, 0, x, 10, r, g, b);

    // Minor ticks every 5 px, 5 px tall
    for (uint16_t x = 5; x < Display_GetWidth(); x += 10)
        Display_DrawLineRGB(x, 0, x, 5, r, g, b);

    // Vertical ruler along left edge
    Display_DrawLineThickRGB(0, 0, 0, Display_GetHeight() - 1, r, g, b, thickness);

    // Major ticks every 10 px, 10 px wide
    for (uint16_t y = 0; y < Display_GetHeight(); y += 10)
        Display_DrawLineRGB(0, y, 10, y, r, g, b);

    // Minor ticks every 5 px, 5 px wide
    for (uint16_t y = 5; y < Display_GetHeight(); y += 10)
        Display_DrawLineRGB(0, y, 5, y, r, g, b);
}

void
Display_DrawChar(uint16_t x, uint16_t y, uint16_t color, char c)
{
    const uint8_t* glyph = Font5x7_GetGlyph(c);
    if (!glyph)
    {
        return;
    }

    uint8_t r5 = (uint8_t)((color >> 11) & 0x1F);
    uint8_t g6 = (uint8_t)((color >> 5) & 0x3F);
    uint8_t b5 = (uint8_t)(color & 0x1F);

    uint8_t r = (uint8_t)(r5 * 255 / 31);
    uint8_t g = (uint8_t)(g6 * 255 / 63);
    uint8_t b = (uint8_t)(b5 * 255 / 31);

    for (uint8_t col = 0; col < FONT5X7_WIDTH; ++col)
    {
        uint8_t col_bits = glyph[col];

        for (uint8_t row = 0; row < FONT5X7_HEIGHT; ++row)
        {
            if (col_bits & (1u << row))
            { // bit0 = top row
                uint16_t px = (uint16_t)(x + col);
                uint16_t py = (uint16_t)(y + row);

                if (px < Display_GetWidth() && py < Display_GetHeight())
                {
                    Display_DrawPixelRGB(px, py, r, g, b);
                }
            }
        }
    }
}

void
Display_DrawString(uint16_t x, uint16_t y, uint16_t color, const char* s)
{
    if (!s)
    {
        return;
    }

    uint16_t cursor_x = x;

    while (*s)
    {
        char c = *s++;

        if (c == '\n')
        {
            y += FONT5X7_HEIGHT + 1u;
            cursor_x = x;
            continue;
        }

        Display_DrawChar(cursor_x, y, color, c);
        cursor_x = (uint16_t)(cursor_x + FONT5X7_WIDTH + 1u);

        if (cursor_x >= Display_GetWidth())
        {
            break;
        }
    }
}
