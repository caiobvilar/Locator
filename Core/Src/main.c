/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include "main.h"

#include "st7789h2.h"

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

// LCD control pin definitions
#define LCD_CS_Pin GPIO_PIN_1
#define LCD_CS_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_2
#define LCD_DC_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_3
#define LCD_RST_GPIO_Port GPIOC

SPI_HandleTypeDef hspi1;

ST7789H2_Object_t hst7789h2;

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
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */
  // Manual GPIO initialization BEFORE clock config (critical for stability)
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  // Manual LCD control pin initialization (PC1=CS, PC2=DC, PC3=RST)
  // MUST be done before clock config for stability
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // LD3 ON - Red
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0xF800); // Red
    HAL_Delay(500);

    // LD4 ON - Green
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0x07E0); // Green
    HAL_Delay(500);

    // LD5 ON - Blue
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0x001F); // Blue
    HAL_Delay(500);

    // LD6 ON - Yellow
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0xFFE0); // Yellow
    HAL_Delay(500);

    // LD3 ON - Magenta
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0xF81F); // Magenta
    HAL_Delay(500);

    // LD4 ON - Cyan
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0x07FF); // Cyan
    HAL_Delay(500);

    // LD5 ON - White
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0xFFFF); // White
    HAL_Delay(500);

    // LD6 ON - Black
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14,
                      GPIO_PIN_RESET);
    LCD_FillScreen(0x0000); // Black
    HAL_Delay(500);
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  // Use HSI (16 MHz internal) instead of HSE to avoid hang
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;             // HSI/8 = 2 MHz
  RCC_OscInitStruct.PLL.PLLN = 96;            // 2 MHz * 96 = 192 MHz
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // 192/2 = 96 MHz SYSCLK
  RCC_OscInitStruct.PLL.PLLQ = 4;             // 192/4 = 48 MHz for USB
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin | LD3_Pin | LD5_Pin | LD6_Pin,
                    GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_SET);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin */
  GPIO_InitStruct.Pin = LD4_Pin | LD3_Pin | LD5_Pin | LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PC1 PC2 PC3 (LCD CS, DC, RST) */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state
   */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
     file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
