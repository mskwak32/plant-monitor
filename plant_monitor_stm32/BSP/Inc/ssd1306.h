#ifndef BSP_INC_SSD1306_H_
#define BSP_INC_SSD1306_H_

#include "main.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_i2c.h"
#include <stdint.h>

/*
OLED Display SSD1306 드라이버
*/

#define SSD1306_WIDTH       128     // 가로 픽셀 수
#define SSD1306_HEIGHT      64      // 세로 픽셀 수
#define SSD1306_I2C_ADDR    0x78    // I2C 주소
#define SSD1306_PAGE_PIXEL  8       // 한 페이지의 픽셀 수

typedef enum {
    SSD1306_BLACK = 0,
    SSD1306_WHITE = 1
} SSD1306_Color;

HAL_StatusTypeDef SSD1306_Init(I2C_HandleTypeDef *hi2c);
void SSD1306_Clear(SSD1306_Color color);
HAL_StatusTypeDef SSD1306_Update(void);
void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_Color color);
void SSD1306_WriteChar(uint8_t x, uint8_t page, char ch);
void SSD1306_WriteString(uint8_t x, uint8_t page, const char *str);

#endif