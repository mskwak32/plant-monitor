#include "plant_monitor.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sensor_monitor.h"
#include "stm32f1xx_hal.h"
#include "water_pump.h"
#include "oled_display.h"
#include "uart_cmd.h"

static WaterPump_Handle pump_handle;
static uint8_t soil_threshold = 40;  // 수분 임계값 (%), 6주차에서 UART로 수신해 변경
static UartCmd_Handle uart_handle;

static void PlantMonitor_HandleUartCmd(void);

void PlantMonitor_Init(
    GPIO_TypeDef* air_sensor_gpio_port,
    uint16_t air_sensor_gpio_pin,
    I2C_HandleTypeDef* hi2c,
    ADC_HandleTypeDef* hadc,
    GPIO_TypeDef* water_pump_gpio_port,
    uint16_t water_pump_gpio_pin,
    UART_HandleTypeDef* huart) {
    
    SensorMonitor_Init(air_sensor_gpio_port, air_sensor_gpio_pin, hadc);
    WaterPump_Init(
        &pump_handle,
        water_pump_gpio_port, water_pump_gpio_pin,
        WATER_PUMP_ON_MS, WATER_PUMP_SOAK_MS
    );
    OledDisplay_Init(hi2c);
    UartCmd_Init(&uart_handle, huart);
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
    PlantMonitor_HandleUartCmd();
}

static void PlantMonitor_HandleUartCmd(void) {
    if (UartCmd_HasLine(&uart_handle)) {
        char buf[UART_CMD_BUF_SIZE];
        UartCmd_GetLine(&uart_handle, buf);
        printf("Received UART command: %s\r\n", buf);

        // 프로토콜: msg={"threshold":30}
        if(strncmp(buf, "msg=", 4) == 0) {
            const char* json = buf + 4;     // "msg=" 이후부터 JSON 문자열
            if(strstr(json, "threshold")) {
                int val;
                if(sscanf(json, "{\"threshold\":%d}", &val) == 1) {
                    if(val >= 0 && val <= 100) {
                        soil_threshold = (uint8_t)val;
                    } else {
                        printf("Invalid threshold: %d\r\n", val);
                    }
                }
            }
        }
    }
}