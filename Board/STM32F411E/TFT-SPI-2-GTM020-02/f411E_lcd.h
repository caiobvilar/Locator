#ifndef F411E_LCD_H
#define F411E_LCD_H

#include "stm32f4xx_hal.h"

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

// LCD control pin definitions
#define LCD_CS_Pin GPIO_PIN_1
#define LCD_CS_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_2
#define LCD_DC_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_3
#define LCD_RST_GPIO_Port GPIOC

extern SPI_HandleTypeDef hspi1;

/* Orientation constants mapped to ST7789 rotations (0-3) */
#define LCD_ORIENTATION_PORTRAIT 2
#define LCD_ORIENTATION_LANDSCAPE 1
#define LCD_ORIENTATION_PORTRAIT_ROT180 0
#define LCD_ORIENTATION_LANDSCAPE_ROT180 3

// LCD control macros
#define LCD_CS_LOW()                                                           \
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_HIGH()                                                          \
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_DC_LOW()                                                           \
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_HIGH()                                                          \
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)
#define LCD_RST_LOW()                                                          \
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)
#define LCD_RST_HIGH()                                                         \
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)

// Function prototypes
void LCD_WriteCommand(uint8_t cmd);
void LCD_WriteData(uint8_t data);
void LCD_WriteData16(uint16_t data);
void LCD_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_FillScreen(uint16_t color);
void LCD_DrawBitmap(uint16_t x, uint16_t y, uint8_t *pBmp);
void LCD_Init(void);
void LCD_SetOrientation(uint32_t orientation);
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                  uint16_t color);

#endif // F411E_LCD_H