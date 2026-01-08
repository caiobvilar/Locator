// main.c
#include "app_main.h"
#include "Display.h" // instead of f411E_lcd.h
#include "main.h"

void
App_Main(void)
{
    // Hardware init done before this
    Display_Init();

    // Test: Fill screen with white
    Display_FillScreen(0xFFFF);
    HAL_Delay(2000);

    while (1)
    {
        // LD3 ON - Orange LED
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
        Display_FillScreen(0xFD20);
        HAL_Delay(500);

        // LD4 ON - Green LED
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
        Display_FillScreen(0x07E0);
        HAL_Delay(500);

        // LD5 ON - Red LED
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15, GPIO_PIN_RESET);
        Display_FillScreen(0xF800);
        HAL_Delay(500);

        // LD6 ON - Blue LED
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
        Display_FillScreen(0x001F);
        HAL_Delay(500);
    }
}
