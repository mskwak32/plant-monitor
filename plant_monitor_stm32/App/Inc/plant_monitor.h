#ifndef APP_INC_PLANT_MONITOR_H_
#define APP_INC_PLANT_MONITOR_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

void PlantMonitor_Init(GPIO_TypeDef *rht_gpio_port,uint16_t rht_gpio_pin,I2C_HandleTypeDef *hi2c,ADC_HandleTypeDef *hadc);
void PlantMonitor_Run(uint8_t soil_threshold);

#endif