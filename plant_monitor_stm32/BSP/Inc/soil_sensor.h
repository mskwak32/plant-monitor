#ifndef BSP_INC_SOIL_SENSOR_H_
#define BSP_INC_SOIL_SENSOR_H_

#include "main.h"

/*
 * 토양 수분 센서(SZH-EK106) 드라이버
 * ADC 채널 0 (PA0)을 사용
 */

/* ADC 핸들은 main.c에서 CubeMX가 생성하므로 extern으로 참조 */
extern ADC_HandleTypeDef hadc1;

typedef struct {
    uint32_t adc_value;   /* 원시 ADC 값 (0 ~ 4095) */
    float    voltage;     /* 전압 (0.0 ~ 3.3V) */
} SoilSensor_Data;

/* ADC 보정(캘리브레이션) 실행 — 부팅 시 1회 호출 */
void SoilSensor_Init(void);

/* 센서 1회 읽기 */
HAL_StatusTypeDef SoilSensor_Read(SoilSensor_Data *data);

#endif /* BSP_INC_SOIL_SENSOR_H_ */
