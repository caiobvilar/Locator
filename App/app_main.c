// main.c
#include "app_main.h"
#include "Display.h" // instead of f411E_lcd.h
#include "main.h"

void
App_Main(void)
{
    // Hardware init done before this
    Display_Init();

    // Clear screen with black background
    Display_FillScreenRGB(0, 0, 0);

    // Draw rulers on X and Y axes (white, 2px thick)
    Display_DrawRulersRGB(255, 255, 255, 2);

    // Draw horizontal line (red, 3px thick)
    Display_DrawLineThickRGB(10, 50, 230, 50, 255, 0, 0, 3);

    // Draw vertical line (green, 3px thick)
    Display_DrawLineThickRGB(120, 60, 120, 200, 0, 255, 0, 3);

    // Draw diagonal line bottom-left to top-right (blue, 3px thick)
    Display_DrawLineThickRGB(20, 150, 220, 100, 0, 0, 255, 3);

    // Draw diagonal line top-left to bottom-right (yellow, 3px thick)
    Display_DrawLineThickRGB(20, 100, 220, 150, 255, 255, 0, 3);

    // Draw diagonal line (magenta, 2px thick)
    Display_DrawLineThickRGB(50, 200, 190, 250, 255, 0, 255, 2);

    // Draw diagonal line (cyan, 2px thick)
    Display_DrawLineThickRGB(190, 200, 50, 250, 0, 255, 255, 2);

    while (1)
    {

        HAL_GPIO_TogglePin(GPIOD, LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin);
        HAL_Delay(200);
    }
}
