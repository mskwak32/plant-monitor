#include "plant_monitor.h"
#include <stdio.h>
#include "sensor_monitor.h"
#include "stm32f1xx_hal.h"
#include "water_pump.h"
#include "oled_display.h"

static WaterPump_Handle pump_handle;
static uint8_t soil_threshold = 40;  // 수분 임계값 (%), 6주차에서 UART로 수신해 변경

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

    OledDisplay_Init(hi2c);
}

void PlantMonitor_Run(void) {
    SensorMonitor_Run();
    const SensorMonitor_Data* data = SensorMonitor_GetData();

    WaterPump_Run(&pump_handle, data->soil_moisture_pct, soil_threshold);

    OledDisplay_Display(
        data->soil_moisture_pct,
        data->air_humidity,
        data->air_temperature,
        soil_threshold,
        WaterPump_GetState(&pump_handle)
    );
}