#ifndef APP_INC_PLANT_MONITOR_H_
#define APP_INC_PLANT_MONITOR_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

void PlantMonitor_Init(
    GPIO_TypeDef *air_sensor_gpio_port,
    uint16_t air_sensor_gpio_pin,
    I2C_HandleTypeDef *hi2c,
    ADC_HandleTypeDef *hadc,
    GPIO_TypeDef *water_pump_gpio_port,
    uint16_t water_pump_gpio_pin
);
void PlantMonitor_Run(void);

#endif