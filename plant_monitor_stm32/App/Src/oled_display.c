#include "oled_display.h"
#include <stdio.h>
#include "ssd1306.h"

#define OLED_DISPLAY_INTERVAL_MS 1000U

static SSD1306_Handle oled_handle;
static uint8_t oled_i2c_addr = 0x78;
static uint32_t last_run_tick = 0;

void OledDisplay_Init(I2C_HandleTypeDef* hi2c) {
    HAL_StatusTypeDef oled_status = SSD1306_Init(&oled_handle, hi2c, oled_i2c_addr);

    if (oled_status != HAL_OK) {
        printf("SSD1306(Display) Init Failed: %d\r\n", oled_status);
    } else {
        printf("SSD1306(Display) Init OK\r\n");
    }
}

void OledDisplay_Display(
    uint8_t soil_moisture_pct,
    float air_humidity,
    float air_temperature,
    uint8_t soil_threshold,
    WaterPump_State pump_state) {
    uint32_t now = HAL_GetTick();

    // 모니터링 주기 체크
    if (now - last_run_tick < OLED_DISPLAY_INTERVAL_MS) {
        return;
    }

    last_run_tick = now;
    char oled_buf[24];

    SSD1306_Clear(&oled_handle, SSD1306_BLACK);

    sprintf(oled_buf, "Soil: %13d%%", soil_moisture_pct);
    SSD1306_WriteString(&oled_handle, 0, 0, oled_buf);

    sprintf(oled_buf, "Threshold: %7d%%", soil_threshold);
    SSD1306_WriteString(&oled_handle, 0, 1, oled_buf);

    sprintf(oled_buf, "Temp:%14.1fC", air_temperature);
    SSD1306_WriteString(&oled_handle, 0, 3, oled_buf);

    sprintf(oled_buf, "Humidity:%10.1f%%", air_humidity);
    SSD1306_WriteString(&oled_handle, 0, 4, oled_buf);

    switch (pump_state) {
        case WATER_PUMP_PUMPING:
            SSD1306_WriteString(&oled_handle, 0, 6, "Pump: ON");
            break;
        case WATER_PUMP_SOAKING:
            SSD1306_WriteString(&oled_handle, 0, 6, "Pump: WAIT");
            break;
        default:
            SSD1306_WriteString(&oled_handle, 0, 6, "Pump: OFF");
            break;
    }

    SSD1306_Update(&oled_handle);
}