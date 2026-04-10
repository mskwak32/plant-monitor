#ifndef BSP_INC_SOIL_SENSOR_H_
#define BSP_INC_SOIL_SENSOR_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#define SOIL_SENSOR_ADC_DRY_DEFAULT     4090
#define SOIL_SENSOR_ADC_WET_DEFAULT     1700
#define SOIL_SENSOR_ADC_MAX             4095

/*
 * 토양 수분 센서(SZH-EK106) 드라이버
 * ADC 채널을 사용
 */

typedef struct {
    uint32_t adc_value;   /* 원시 ADC 값 (0 ~ 4095) */
    float    voltage;     /* 전압 (0.0 ~ 3.3V) */
} SoilSensor_Data;

typedef struct {
    ADC_HandleTypeDef *hadc;
    SoilSensor_Data data;
    uint16_t adc_dry;       /**< 건조할 때 ADC raw value */
    uint16_t adc_wet;       /**< 완전 젖었을 때 ADC raw value */
} SoilSensor_Handle;

HAL_StatusTypeDef SoilSensor_Init(SoilSensor_Handle *handle, ADC_HandleTypeDef *hadc, uint16_t adc_dry, uint16_t adc_wet);
HAL_StatusTypeDef SoilSensor_Read(SoilSensor_Handle *handle);

#endif /* BSP_INC_SOIL_SENSOR_H_ */
