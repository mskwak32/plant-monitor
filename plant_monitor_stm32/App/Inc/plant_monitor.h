#ifndef APP_INC_PLANT_MONITOR_H_
#define APP_INC_PLANT_MONITOR_H_

#include "main.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_i2c.h"

void PlantMonitor_Init(I2C_HandleTypeDef *hi2c, ADC_HandleTypeDef *hadc);
void PlantMonitor_Run(void);

#endif