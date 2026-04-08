#include "soil_sensor.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_def.h"

static ADC_HandleTypeDef *hadc_;
/*
 * ADC 보정(캘리브레이션) 실행
 * STM32F1 ADC는 내부 오프셋 오차가 있어서, 최초 1회 보정이 필요함.
 * 보정하지 않으면 측정값에 최대 ±2LSB 오차가 생길 수 있음.
 */
HAL_StatusTypeDef SoilSensor_Init(ADC_HandleTypeDef *hadc) {
    hadc_ = hadc;
    return HAL_ADCEx_Calibration_Start(hadc_);
}

/*
 * 토양 수분 센서 1회 읽기
 * ADC 변환 시작 → 완료 대기 → 값 읽기 → 정지 순서로 동작
 */
HAL_StatusTypeDef SoilSensor_Read(SoilSensor_Data *data) {
    HAL_ADC_Start(hadc_);

    // 변환 완료 전에 값을 읽으면 이전 변환의 값이 나올 수 있으므로
    // PollForConversion으로 반드시 완료를 확인해야 함
    if (HAL_ADC_PollForConversion(hadc_, HAL_MAX_DELAY) != HAL_OK) {
        HAL_ADC_Stop(hadc_);
        return HAL_ERROR;
    }

    data->adc_value = HAL_ADC_GetValue(hadc_);
    data->voltage   = data->adc_value * 3.3f / (float)SOIL_SENSOR_ADC_DRY;

    HAL_ADC_Stop(hadc_);
    return HAL_OK;
}
