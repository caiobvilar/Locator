#include "f411E_lcd.h"
#include "st7789h2.h"
#include "stm32f4xx_hal.h"

extern SPI_HandleTypeDef hspi1;
ST7789H2_Object_t hst7789h2;
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
  ST7789H2_DrawBitmap(&hst7789h2, x, y, pBmp);
}

// ST7789H2 driver I/O callbacks
int32_t ST7789H2_IO_Init(void) {
  // Manual initialization sequence (proven to work with this display)
  // Software reset
  LCD_WriteCommand(0x01);
  HAL_Delay(150);

  // Sleep out
  LCD_WriteCommand(0x11);
  HAL_Delay(120);

  // Color mode - 16bit RGB565
  LCD_WriteCommand(0x3A);
  LCD_WriteData(0x55);

  // Memory data access control
  LCD_WriteCommand(0x36);
  LCD_WriteData(0x00); // Normal orientation

  // Column address set (0 to 239)
  LCD_WriteCommand(0x2A);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0xEF);

  // Row address set (0 to 319)
  LCD_WriteCommand(0x2B);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x01);
  LCD_WriteData(0x3F);

  // Display inversion on
  LCD_WriteCommand(0x21);

  // Normal display mode
  LCD_WriteCommand(0x13);

  // Display on
  LCD_WriteCommand(0x29);
  HAL_Delay(100);

  return ST7789H2_OK;
}

int32_t ST7789H2_IO_DeInit(void) { return ST7789H2_OK; }

int32_t ST7789H2_IO_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
                             uint32_t Length) {
  // Write command
  LCD_WriteCommand((uint8_t)Reg);

  // Write data if present
  if (Length > 0 && pData != NULL) {
    LCD_DC_HIGH();
    LCD_CS_LOW();
    HAL_SPI_Transmit(&hspi1, pData, Length, HAL_MAX_DELAY);
    LCD_CS_HIGH();
  }
  return ST7789H2_OK;
}

int32_t ST7789H2_IO_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
                            uint32_t Length) {
  LCD_WriteCommand((uint8_t)Reg);

  if (Length > 0 && pData != NULL) {
    LCD_DC_HIGH();
    LCD_CS_LOW();
    HAL_SPI_Receive(&hspi1, pData, Length, HAL_MAX_DELAY);
    LCD_CS_HIGH();
  }
  return ST7789H2_OK;
}

int32_t ST7789H2_IO_SendData(uint8_t *pData, uint32_t Length) {
  // SendData in ST7789H2 library is used to send commands (first byte)
  // The first byte is a command, rest are data
  if (Length > 0 && pData != NULL) {
    // Send first byte as command
    LCD_WriteCommand(pData[0]);

    // Send remaining bytes as data if any
    if (Length > 1) {
      LCD_DC_HIGH();
      LCD_CS_LOW();
      HAL_SPI_Transmit(&hspi1, &pData[1], Length - 1, HAL_MAX_DELAY);
      LCD_CS_HIGH();
    }
  }
  return ST7789H2_OK;
}

int32_t ST7789H2_IO_GetTick(void) { return (int32_t)HAL_GetTick(); }

void LCD_Init(void) {
  // Hardware reset sequence (must be done before registering callbacks)
  LCD_CS_HIGH();
  LCD_RST_HIGH();
  HAL_Delay(10);
  LCD_RST_LOW();
  HAL_Delay(20);
  LCD_RST_HIGH();
  HAL_Delay(120);

  // Register I/O callbacks for ST7789H2 driver
  // io.Init contains our manual initialization sequence
  ST7789H2_IO_t io;
  io.Init = ST7789H2_IO_Init;
  io.DeInit = ST7789H2_IO_DeInit;
  io.Address = 0;
  io.WriteReg = ST7789H2_IO_WriteReg;
  io.ReadReg = ST7789H2_IO_ReadReg;
  io.SendData = ST7789H2_IO_SendData;
  io.GetTick = ST7789H2_IO_GetTick;

  // This will call io.Init() which executes our manual initialization
  ST7789H2_RegisterBusIO(&hst7789h2, &io);
}
