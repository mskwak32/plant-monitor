#ifndef BSP_INC_SSD1306_H_
#define BSP_INC_SSD1306_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

/*
OLED Display SSD1306 드라이버
128x64용
*/

#define SSD1306_WIDTH       128     // 가로 픽셀 수
#define SSD1306_HEIGHT      64      // 세로 픽셀 수
#define SSD1306_PAGE_PIXEL  8       // 한 페이지의 픽셀 수

typedef enum {
    SSD1306_BLACK = 0,
    SSD1306_WHITE = 1
} SSD1306_Color;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t i2c_addr;
    uint8_t framebuffer[SSD1306_WIDTH * SSD1306_HEIGHT / SSD1306_PAGE_PIXEL];     // 128x64 / 8 = 1024bytes
} SSD1306_Handle;

HAL_StatusTypeDef SSD1306_Init(SSD1306_Handle *handle, I2C_HandleTypeDef *hi2c, uint8_t addr);
void SSD1306_Clear(SSD1306_Handle *handle, SSD1306_Color color);
HAL_StatusTypeDef SSD1306_Update(SSD1306_Handle *handle);
void SSD1306_DrawPixel(SSD1306_Handle *handle, uint8_t x, uint8_t y, SSD1306_Color color);
void SSD1306_WriteChar(SSD1306_Handle *handle, uint8_t x, uint8_t page, char ch);
void SSD1306_WriteString(SSD1306_Handle *handle, uint8_t x, uint8_t page, const char *str);

#endif