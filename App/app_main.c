// App/app_main.c

#include "app_main.h"
#include "Display.h"
#include "f411E_lcd.h"
#include "main.h"

void
App_Main(void)
{
    // Hardware init done before this

    Display_Init();
    Display_SetOrientation(LCD_ORIENTATION_LANDSCAPE_ROT180); // or LCD_ORIENTATION_LANDSCAPE if you expose it

    uint16_t w = Display_GetWidth();
    uint16_t h = Display_GetHeight();

    // Clear screen with black background
    Display_FillScreenRGB(0, 0, 0);

    // Draw rulers on X and Y axes (white, 2px thick)
    Display_DrawRulersRGB(255, 255, 255, 2);

    // Some convenient reference points
    uint16_t margin = w / 12; // ~20px on 240‑wide
    uint16_t mid_x = w / 2;
    // uint16_t mid_y = h / 2;
    uint16_t near_max = w - margin;
    uint16_t low_y = h - (h / 4);
    uint16_t high_y = h / 4;

    // Horizontal line (red, 3px thick), across most of the width
    Display_DrawLineThickRGB(margin, h / 6, near_max, h / 6, 255, 0, 0, 3);

    // Vertical line (green, 3px thick) through center
    Display_DrawLineThickRGB(mid_x, high_y, mid_x, h - margin, 0, 255, 0, 3);

    // Diagonal bottom-left to top-right (blue, 3px thick)
    Display_DrawLineThickRGB(margin, low_y, near_max, high_y, 0, 0, 255, 3);

    // Diagonal top-left to bottom-right (yellow, 3px thick)
    Display_DrawLineThickRGB(margin, high_y, near_max, low_y, 255, 255, 0, 3);

    // Short diagonal near bottom (magenta, 2px thick)
    Display_DrawLineThickRGB(w / 4, h - (h / 6), (3 * w) / 4, h - (h / 12), 255, 0, 255, 2);

    // Short diagonal near bottom (cyan, 2px thick), opposite direction
    Display_DrawLineThickRGB((3 * w) / 4, h - (h / 6), w / 4, h - (h / 12), 0, 255, 255, 2);

    while (1)
    {
        HAL_GPIO_TogglePin(GPIOD, LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin);
        HAL_Delay(200);
    }
}
