#include "f411E_lcd.h"
#include "st7789.h"
#include "stm32f4xx_hal.h"

extern SPI_HandleTypeDef hspi1;
void LCD_WriteCommand(uint8_t cmd) {
  LCD_DC_LOW();
  LCD_CS_LOW();
  HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
  LCD_CS_HIGH();
}

void LCD_WriteData(uint8_t data) {
  LCD_DC_HIGH();
  LCD_CS_LOW();
  HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
  LCD_CS_HIGH();
}

void LCD_WriteData16(uint16_t data) {
  uint8_t buffer[2];
  buffer[0] = (data >> 8) & 0xFF;
  buffer[1] = data & 0xFF;
  LCD_DC_HIGH();
  LCD_CS_LOW();
  HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
  LCD_CS_HIGH();
}

void LCD_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  LCD_WriteCommand(0x2A);
  LCD_WriteData16(x0);
  LCD_WriteData16(x1);
  LCD_WriteCommand(0x2B);
  LCD_WriteData16(y0);
  LCD_WriteData16(y1);
  LCD_WriteCommand(0x2C);
}

void LCD_FillScreen(uint16_t color) {
  LCD_SetAddressWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
  uint8_t hi = color >> 8;
  uint8_t lo = color & 0xFF;
  LCD_CS_LOW();
  LCD_DC_HIGH();
  for (uint32_t i = 0; i < (uint32_t)LCD_WIDTH * LCD_HEIGHT; i++) {
    HAL_SPI_Transmit(&hspi1, &hi, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, &lo, 1, HAL_MAX_DELAY);
  }
  LCD_CS_HIGH();
}

void LCD_DrawBitmap(uint16_t x, uint16_t y, uint8_t *pBmp) {
  (void)pBmp;
  /* TODO: implement BMP drawing if required */
  ST7789_Fill(x, y, x + 49, y + 49, RED);
}

// ST7789H2 driver I/O callbacks
void LCD_Init(void) {
  /* Initialize the new ST7789 driver */
  ST7789_Init();
}

void LCD_SetOrientation(uint32_t orientation) {
  uint8_t rot = 2;
  switch (orientation) {
  case LCD_ORIENTATION_PORTRAIT:
    rot = 2;
    break;
  case LCD_ORIENTATION_LANDSCAPE:
    rot = 1;
    break;
  case LCD_ORIENTATION_PORTRAIT_ROT180:
    rot = 0;
    break;
  case LCD_ORIENTATION_LANDSCAPE_ROT180:
    rot = 3;
    break;
  default:
    rot = 2;
    break;
  }
  ST7789_SetRotation(rot);
}

void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
  ST7789_DrawPixel(x, y, color);
}

void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                  uint16_t color) {
  if (w == 0 || h == 0)
    return;
  ST7789_Fill(x, y, x + w - 1, y + h - 1, color);
}
