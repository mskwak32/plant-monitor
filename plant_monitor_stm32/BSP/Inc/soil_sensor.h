#ifndef BSP_INC_SOIL_SENSOR_H_
#define BSP_INC_SOIL_SENSOR_H_

#include "main.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_def.h"

#define SOIL_SENSOR_ADC_DRY     4090
#define SOIL_SENSOR_ADC_WET     1700

/*
 * 토양 수분 센서(SZH-EK106) 드라이버
 * ADC 채널을 사용
 */

typedef struct {
    uint32_t adc_value;   /* 원시 ADC 값 (0 ~ 4095) */
    float    voltage;     /* 전압 (0.0 ~ 3.3V) */
} SoilSensor_Data;

HAL_StatusTypeDef SoilSensor_Init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef SoilSensor_Read(SoilSensor_Data *data);

#endif /* BSP_INC_SOIL_SENSOR_H_ */
