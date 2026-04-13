#ifndef APP_INC_SENSOR_MONITOR_H_
#define APP_INC_SENSOR_MONITOR_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

typedef struct {
    HAL_StatusTypeDef soil_sensor;
    HAL_StatusTypeDef air_sensor;
} SensorMonitor_Status;

typedef struct {
    uint8_t soil_moisture_pct;  // 토양습도 퍼센트
    float air_humidity;
    float air_temperature;
} SensorMonitor_Data;

SensorMonitor_Status SensorMonitor_Init(
    GPIO_TypeDef *air_sensor_gpio_port,
    uint16_t air_sensor_gpio_pin,
    ADC_HandleTypeDef *hadc
);

void SensorMonitor_Run(void);

const SensorMonitor_Data *SensorMonitor_GetData(void);

#endif