#include "main.h"
#include "app_main.h"
#include "f411E_lcd.h" // your board BSP

void App_Main(void) {
  // Hardware initialization is done in main.c before calling App_Main
  LCD_Init();
  
  // Test: Fill screen with white to verify LCD is working
  LCD_FillScreen(0xFFFF); // White
  HAL_Delay(2000);
  
  while (1) {
    // LD3 ON - Orange LED
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0xFD20); // Orange
    HAL_Delay(500);

    // LD4 ON - Green LED
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0x07E0); // Green
    HAL_Delay(500);

    // LD5 ON - Red LED
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0xF800); // Red
    HAL_Delay(500);

    // LD6 ON - Blue LED
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0x001F); // Blue
    HAL_Delay(500);
  }
}
