#ifndef F411E_LCD_H
#define F411E_LCD_H

#include "st7789.h"
#include "stm32f4xx_hal.h"

/* Geometry */
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

/* Control pins (match CubeMX) */
#define LCD_CS_Pin GPIO_PIN_1
#define LCD_CS_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_2
#define LCD_DC_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_3
#define LCD_RST_GPIO_Port GPIOC

extern SPI_HandleTypeDef hspi1;

/* Pin macros */
#define LCD_CS_LOW() HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_HIGH() HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_DC_LOW() HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_HIGH() HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)
#define LCD_RST_LOW() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)
#define LCD_RST_HIGH() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)

/* Orientation constants */
#define LCD_ORIENTATION_PORTRAIT 0
#define LCD_ORIENTATION_LANDSCAPE 1
#define LCD_ORIENTATION_PORTRAIT_ROT180 2
#define LCD_ORIENTATION_LANDSCAPE_ROT180 3

/* BSP API */
void
LCD_Init(void);
void
LCD_SetOrientation(uint32_t orientation);
st7789_t*
F411E_LCD_GetHandle(void);

#endif
