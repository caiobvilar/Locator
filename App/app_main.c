// App/app_main.c

#include "app_main.h"
#include "Display.h"
#include "f411E_lcd.h"
#include "main.h"
#include <stdio.h>

void
App_Main(void)
{
    // Hardware init done before this

    Display_Init();
    Display_SetOrientation(LCD_ORIENTATION_PORTRAIT);

    uint16_t w = Display_GetWidth();  // 240
    uint16_t h = Display_GetHeight(); // 320
    char buf1[8];
    char buf2[8];
    snprintf(buf1, sizeof buf1, "%u", Display_GetWidth());
    snprintf(buf2, sizeof buf2, "%u", Display_GetHeight());
    // Clear screen with black background
    Display_FillScreenRGB(0, 0, 0);

    // Draw rulers on X and Y axes (white, 2px thick)
    // Display_DrawRulersRGB(255, 255, 255, 2);

    // Some convenient reference points

    Display_DrawString(0, 0, COLOR_CYAN, buf2);           // HEIGHT
    Display_DrawString(w - 30, h - 30, COLOR_CYAN, buf1); // WIDTH
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOD, LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin);
        HAL_Delay(200);
    }
}
