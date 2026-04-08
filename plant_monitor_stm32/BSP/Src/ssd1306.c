#include "ssd1306.h"
#include "ssd1306_font.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_i2c.h"
#include <stdint.h>
#include <string.h>

static I2C_HandleTypeDef *hi2c_;
static uint8_t framebuffer_[SSD1306_WIDTH * SSD1306_HEIGHT / SSD1306_PAGE_PIXEL];   // 1frame: 1024bytes

HAL_StatusTypeDef SSD1306_Init(I2C_HandleTypeDef *hi2c) {
    hi2c_ = hi2c
;
    HAL_Delay(100); // OLED 전원 안정화 대기

    uint8_t cmds[] = {
        0x00,             // Control byte: 이후 모든 바이트는 Command
        0xAE,              // Display OFF (초기화 중 화면 끄기)
        0xD5, 0x80,    // 클럭 분주비 설정 (기본값)
        0xA8, 0x3F,    // Multiplex ratio: 64행 (0x3F = 63)
        0xD3, 0x00,    // Display offset: 없음
        0x40,              // Start line: 0번 행부터 표시
        0x8D, 0x14,   // Charge pump: 활성화 (내부 승압 ON)
        0x20, 0x00,  // 메모리 주소 방식: Horizontal (좌→우, 위→아래 자동 이동)
        0xA1,             // Segment remap: col127=SEG0 (좌우 반전 보정)
        0xC8,             // COM scan 방향 반전 (상하 반전 보정)
        0xDA, 0x12,  // COM 핀 하드웨어 설정 (128×64 패널용)
        0x81, 0xCF,  // 밝기(Contrast): 0xCF
        0xD9, 0xF1,  // Pre-charge period
        0xDB, 0x30,  // VCOMH deselect level
        0xA4,             // GDDRAM 내용을 화면에 표시 (RAM display resume)
        0xA6,             // 정상 표시 (1=켜짐, 반전 아님)
        0xAF,             // Display ON
    };

    return HAL_I2C_Master_Transmit(hi2c_, SSD1306_I2C_ADDR, cmds, sizeof(cmds), HAL_MAX_DELAY);
}

/*
framebuffer만 비움. 화면에 반영하려면 Update해줘야 함.
*/
void SSD1306_Clear(SSD1306_Color color) {
    memset(framebuffer_, (color == SSD1306_WHITE ? 0xFF : 0x00), sizeof(framebuffer_));
}

/*
framebuffer_의 값으로 화면 업데이트.
framebuffer_에 화면 전체의 픽셀 데이터가 있으니 화면 전체 업데이트
*/
HAL_StatusTypeDef SSD1306_Update(void) {
    // 쓸 영역 지정: 열 0~127, page 0~7 (화면 전체)
    uint8_t addr_cmds[] = {
        0x00,                          // Control: commands
        0x21, 0x00, SSD1306_WIDTH - 1, // Set Column Address: 0~127
        0x22, 0x00, (SSD1306_HEIGHT / SSD1306_PAGE_PIXEL) - 1, // Set Page Address: 0~7
    };

    if (HAL_I2C_Master_Transmit(hi2c_, SSD1306_I2C_ADDR, addr_cmds, sizeof(addr_cmds), HAL_MAX_DELAY) != HAL_OK) {
        return HAL_ERROR;
    }

    // 프레임 버퍼 전송
    static uint8_t data_buf[1 + SSD1306_WIDTH * SSD1306_HEIGHT / SSD1306_PAGE_PIXEL];
    data_buf[0] = 0x40; // Control: 이후 모든 바이트는 GDDRAM 데이터
    memcpy(&data_buf[1], framebuffer_, sizeof(framebuffer_));
    return HAL_I2C_Master_Transmit(hi2c_, SSD1306_I2C_ADDR, data_buf, sizeof(data_buf), HAL_MAX_DELAY);
}

void SSD1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_Color color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;

    if (color == SSD1306_WHITE) {
        framebuffer_[(y / SSD1306_PAGE_PIXEL) * SSD1306_WIDTH + x] |= (1 << (y % SSD1306_PAGE_PIXEL));
    } else {
        framebuffer_[(y / SSD1306_PAGE_PIXEL) * SSD1306_WIDTH + x] &= ~(1 << (y % SSD1306_PAGE_PIXEL));
    }
}


/**
    프레임버퍼의 (x, page) 위치에 1글자를 그립니다.
    @param x x 좌표. 한 글자당 6개 차지
    - 5x8 픽셀 폰트 데이터(font5x8)를 프레임버퍼에 복사.
    - 범위 체크(경계 검사)를 하므로, 유효한 좌표만 그립니다.
    - 비인쇄 문자('\n', '\t' 등)는 '?'로 대체합니다.
 */
void SSD1306_WriteChar(uint8_t x, uint8_t page, char ch) {
    if(ch < 0x20 || ch > 0x7E) ch = '?';
    if(x + 6 > SSD1306_WIDTH || page >= SSD1306_HEIGHT / SSD1306_PAGE_PIXEL) return;

    const uint8_t *glyph = font5x8[ch - 0x20];
    uint16_t base = page * SSD1306_WIDTH + x;

    for (uint8_t i = 0; i < 5; i++) {
        framebuffer_[base + i] = glyph[i];
    }
    framebuffer_[base + 5] = 0x00; // 글자 사이 1px 여백
}

/**
    @param x x좌표. 한 글자당 6개 차지
*/
void SSD1306_WriteString(uint8_t x, uint8_t page, const char *str) {
    while(*str && x + 6 <= SSD1306_WIDTH) {
        SSD1306_WriteChar(x, page, *str++);
        x+= 6;  // 문자 너비 5px + 여백 1px
    }
}