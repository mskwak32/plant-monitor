#include "plant_monitor.h"

#include <stdio.h>

#include "sensor_monitor.h"
#include "ssd1306.h"
#include "stm32f1xx_hal.h"
#include "water_pump.h"

static SSD1306_Handle oled_handle;
static uint8_t oled_i2c_addr = 0x78;
static WaterPump_Handle pump_handle;
static uint8_t soil_threshold = 40;  // 수분 임계값 (%), 6주차에서 UART로 수신해 변경

static void OLED_Display(
    uint8_t soil_moisture_ptc,
    float air_humidity,
    float air_temperature,
    uint8_t soil_threshold,
    WaterPump_State pump_state);

void PlantMonitor_Init(
    GPIO_TypeDef* air_sensor_gpio_port,
    uint16_t air_sensor_gpio_pin,
    I2C_HandleTypeDef* hi2c,
    ADC_HandleTypeDef* hadc,
    GPIO_TypeDef* water_pump_gpio_port,
    uint16_t water_pump_gpio_pin) {
    SensorMonitor_Init(air_sensor_gpio_port, air_sensor_gpio_pin, hadc);

    WaterPump_Init(
        &pump_handle,
        water_pump_gpio_port, water_pump_gpio_pin,
        WATER_PUMP_ON_MS, WATER_PUMP_SOAK_MS
    );

    HAL_StatusTypeDef oled_status = SSD1306_Init(&oled_handle, hi2c, oled_i2c_addr);

    if (oled_status != HAL_OK) {
        printf("SSD1306(Display) Init Failed: %d\r\n", oled_status);
    } else {
        printf("SSD1306(Display) Init OK\r\n");
    }
}

void PlantMonitor_Run(void) {
    SensorMonitor_Run();
    const SensorMonitor_Data* data = SensorMonitor_GetData();

    WaterPump_Run(&pump_handle, data->soil_moisture_pct, soil_threshold);

    OLED_Display(
        data->soil_moisture_pct,
        data->air_humidity,
        data->air_temperature,
        soil_threshold,
        WaterPump_GetState(&pump_handle)
    );

    HAL_Delay(2000);
}

static void OLED_Display(
    uint8_t soil_moisture_ptc,
    float air_humidity,
    float air_temperature,
    uint8_t soil_threshold,
    WaterPump_State pump_state) {
    char oled_buf[24];

    SSD1306_Clear(&oled_handle, SSD1306_BLACK);

    sprintf(oled_buf, "Soil: %13d%%", soil_moisture_ptc);
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