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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// CS/DC/RST helpers on PC1/PC2/PC3 (check these match your CubeMX pins)
static inline void
ST_CS_L(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
}

static inline void
ST_CS_H(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

static inline void
ST_DC_CMD(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
}

static inline void
ST_DC_DATA(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
}

static inline void
ST_RST_L(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
}

static inline void
ST_RST_H(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
}

extern SPI_HandleTypeDef hspi1;

static void
st_write(uint8_t dc, const uint8_t* p, uint16_t n)
{
    if (dc)
        ST_DC_DATA();
    else
        ST_DC_CMD();
    ST_CS_L();
    HAL_SPI_Transmit(&hspi1, (uint8_t*)p, n, HAL_MAX_DELAY);
    ST_CS_H();
}

/* Convert standard RGB (8-bit per channel) to display GBR format (3 bytes) */
static inline void
rgb_to_gbr(uint8_t r, uint8_t g, uint8_t b, uint8_t* out)
{
    out[0] = g; // Green byte
    out[1] = b; // Blue byte
    out[2] = r; // Red byte
}

/* Set display window (CASET and RASET) */
static void
st_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t cmd, data[4];

    // 0x2A - CASET (Column Address Set)
    cmd = 0x2A;
    st_write(0, &cmd, 1);
    data[0] = (uint8_t)(x0 >> 8);
    data[1] = (uint8_t)(x0 & 0xFF);
    data[2] = (uint8_t)(x1 >> 8);
    data[3] = (uint8_t)(x1 & 0xFF);
    st_write(1, data, 4);

    // 0x2B - RASET (Row Address Set)
    cmd = 0x2B;
    st_write(0, &cmd, 1);
    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)(y0 & 0xFF);
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)(y1 & 0xFF);
    st_write(1, data, 4);
}

/* Draw a pixel at (x, y) with given color (RGB) */
static void
st_draw_pixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b)
{
    st_set_window(x, y, x, y);

    uint8_t cmd = 0x2C; // RAMWR
    st_write(0, &cmd, 1);

    uint8_t px[3];
    rgb_to_gbr(r, g, b, px);

    ST_DC_DATA();
    ST_CS_L();
    HAL_SPI_Transmit(&hspi1, px, 3, HAL_MAX_DELAY);
    ST_CS_H();
}

/* Draw a line from (x0, y0) to (x1, y1) using Bresenham's algorithm */
static void
st_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b)
{
    int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int16_t dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    int16_t x = x0;
    int16_t y = y0;

    while (1)
    {
        st_draw_pixel((uint16_t)x, (uint16_t)y, r, g, b);

        if (x == x1 && y == y1)
            break;

        int16_t e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}

/* Forward declaration */
static void
st_draw_line_thick(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b);

/* Draw a ruler/scale at origin with tick marks */
static void
st_draw_ruler(uint8_t r, uint8_t g, uint8_t b)
{
    // Draw horizontal ruler along top edge (y=0 to y=10)
    // Draw baseline
    st_draw_line_thick(0, 0, 239, 0, r, g, b);

    // Draw tick marks every 10 pixels (major ticks - 10 pixels tall)
    for (uint16_t x = 0; x < 240; x += 10)
    {
        st_draw_line(x, 0, x, 10, r, g, b);
    }

    // Draw tick marks every 5 pixels (minor ticks - 5 pixels tall)
    for (uint16_t x = 5; x < 240; x += 10)
    {
        st_draw_line(x, 0, x, 5, r, g, b);
    }

    // Draw vertical ruler along left edge (x=0 to x=10)
    // Draw baseline
    st_draw_line_thick(0, 0, 0, 319, r, g, b);

    // Draw tick marks every 10 pixels (major ticks - 10 pixels wide)
    for (uint16_t y = 0; y < 320; y += 10)
    {
        st_draw_line(0, y, 10, y, r, g, b);
    }

    // Draw tick marks every 5 pixels (minor ticks - 5 pixels wide)
    for (uint16_t y = 5; y < 320; y += 10)
    {
        st_draw_line(0, y, 5, y, r, g, b);
    }
}

/* Draw a thick line (5 pixels wide) */
static void
st_draw_line_thick(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b)
{
    // Draw center line
    st_draw_line(x0, y0, x1, y1, r, g, b);

    // Determine if line is more horizontal or vertical
    int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int16_t dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);

    if (dx > dy)
    {
        // More horizontal - draw parallel lines above and below
        for (int offset = 1; offset <= 2; offset++)
        {
            if (y0 >= offset)
                st_draw_line(x0, y0 - offset, x1, y1 - offset, r, g, b);
            if (y0 + offset < 320)
                st_draw_line(x0, y0 + offset, x1, y1 + offset, r, g, b);
        }
    }
    else
    {
        // More vertical - draw parallel lines left and right
        for (int offset = 1; offset <= 2; offset++)
        {
            if (x0 >= offset)
                st_draw_line(x0 - offset, y0, x1 - offset, y1, r, g, b);
            if (x0 + offset < 240)
                st_draw_line(x0 + offset, y0, x1 + offset, y1, r, g, b);
        }
    }
}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void
SystemClock_Config(void);
static void
MX_GPIO_Init(void);
static void
MX_SPI1_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Common write helper


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int
main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick.
     */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_SPI1_Init();

    /* USER CODE BEGIN 2 */

    // Hardware Reset sequence
    ST_RST_L();
    HAL_Delay(10);
    ST_RST_H();
    HAL_Delay(120);

    // ST7789V Initialization Sequence
    /* CRITICAL: This configuration works with:
       - SPI Mode 3 (CPOL=1, CPHA=1)
       - COLMOD = 0x66 (18-bit RGB888, not 16-bit!)
       - MADCTL = 0x00 (RGB mode, no BGR)
       - Pixel bytes sent as {G, B, R} order (GBR)
       Example: pure red = {0x00, 0xFF, 0xFF}
       Do NOT mix with 16-bit mode or BGR settings! */
    uint8_t cmd, data;

    // 0x01 - SWRESET (Software Reset)
    cmd = 0x01;
    st_write(0, &cmd, 1);
    HAL_Delay(150);

    // 0x11 - SLPOUT (Sleep Out)
    cmd = 0x11;
    st_write(0, &cmd, 1);
    HAL_Delay(120);

    // 0x3A - COLMOD (Interface Pixel Format) - 18-bit RGB666
    cmd = 0x3A;
    st_write(0, &cmd, 1);
    data = 0x66; // 18-bit
    st_write(1, &data, 1);

    // 0x36 - MADCTL (Memory Access Control) - RGB mode (no BGR)
    cmd = 0x36;
    st_write(0, &cmd, 1);
    data = 0x00; // RGB mode
    st_write(1, &data, 1);

    // 0x2A - CASET (Column Address Set) - 0 to 239
    cmd = 0x2A;
    st_write(0, &cmd, 1);
    uint8_t caset[4] = {0x00, 0x00, 0x00, 0xEF}; // 239
    st_write(1, caset, 4);

    // 0x2B - RASET (Row Address Set) - 0 to 319
    cmd = 0x2B;
    st_write(0, &cmd, 1);
    uint8_t raset[4] = {0x00, 0x00, 0x01, 0x3F}; // 319
    st_write(1, raset, 4);

    // 0x29 - DISPON (Display ON)
    cmd = 0x29;
    st_write(0, &cmd, 1);
    HAL_Delay(100);

    // 0x2C - RAMWR (Write Memory Start)
    cmd = 0x2C;
    st_write(0, &cmd, 1);

    // Clear screen with black (fill all pixels)
    uint8_t px[3] = {0x00, 0x00, 0x00}; // GBR order for black
    ST_DC_DATA();
    ST_CS_L();
    for (uint32_t i = 0; i < 240UL * 320UL; ++i)
    {
        HAL_SPI_Transmit(&hspi1, px, 3, HAL_MAX_DELAY);
    }
    ST_CS_H();

    // Draw scale at origin (white dots every 5 pixels)
    HAL_Delay(500);
    st_draw_ruler(255, 255, 255);

    // Draw some lines in different colors (3 pixels thick)
    st_draw_line_thick(10, 10, 230, 10, 255, 0, 0);    // Red horizontal line
    st_draw_line_thick(10, 20, 230, 20, 0, 255, 0);    // Green horizontal line
    st_draw_line_thick(10, 30, 230, 30, 0, 0, 255);    // Blue horizontal line
    st_draw_line_thick(10, 10, 10, 100, 255, 255, 0);  // Yellow vertical line
    st_draw_line_thick(50, 50, 200, 150, 255, 0, 255); // Magenta diagonal line
    st_draw_line_thick(50, 150, 200, 50, 0, 255, 255); // Cyan diagonal line

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */
        /* Toggle all LEDs to verify code is running */
        HAL_GPIO_TogglePin(GPIOD, LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin);
        HAL_Delay(200);

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void
SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void
MX_SPI1_Init(void)
{

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    /* NOTE: ST7789V on GMT020-02 requires SPI Mode 3 (CPOL=1, CPHA=2EDGE)
       This must match the working configuration - do not change without testing! */
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH; /* CPOL = 1 */
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;      /* CPHA = 1 (Mode 3) */
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
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
static void
MX_GPIO_Init(void)
{
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
    HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, CS_Pin | DC_Pin | RST_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, LD4_Pin | LD3_Pin | LD5_Pin | LD6_Pin | Audio_RST_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : DATA_Ready_Pin */
    GPIO_InitStruct.Pin = DATA_Ready_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DATA_Ready_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : CS_I2C_SPI_Pin */
    GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : INT1_Pin INT2_Pin MEMS_INT2_Pin */
    GPIO_InitStruct.Pin = INT1_Pin | INT2_Pin | MEMS_INT2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pins : OTG_FS_PowerSwitchOn_Pin CS_Pin DC_Pin RST_Pin */
    GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin | CS_Pin | DC_Pin | RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : PA0 */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : I2S3_WS_Pin */
    GPIO_InitStruct.Pin = I2S3_WS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(I2S3_WS_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : CLK_IN_Pin PB12 */
    GPIO_InitStruct.Pin = CLK_IN_Pin | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                             Audio_RST_Pin */
    GPIO_InitStruct.Pin = LD4_Pin | LD3_Pin | LD5_Pin | LD6_Pin | Audio_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*Configure GPIO pins : I2S3_MCK_Pin I2S3_SCK_Pin I2S3_SD_Pin */
    GPIO_InitStruct.Pin = I2S3_MCK_Pin | I2S3_SCK_Pin | I2S3_SD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : VBUS_FS_Pin */
    GPIO_InitStruct.Pin = VBUS_FS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(VBUS_FS_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : OTG_FS_ID_Pin OTG_FS_DM_Pin OTG_FS_DP_Pin */
    GPIO_InitStruct.Pin = OTG_FS_ID_Pin | OTG_FS_DM_Pin | OTG_FS_DP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
    GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : Audio_SCL_Pin Audio_SDA_Pin */
    GPIO_InitStruct.Pin = Audio_SCL_Pin | Audio_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */

    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void
Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
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
void
assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
       line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
