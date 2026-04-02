#include "soil_sensor.h"

/*
 * ADC 보정(캘리브레이션) 실행
 * STM32F1 ADC는 내부 오프셋 오차가 있어서, 최초 1회 보정이 필요함.
 * 보정하지 않으면 측정값에 최대 ±2LSB 오차가 생길 수 있음.
 */
void SoilSensor_Init(void) {
    HAL_ADCEx_Calibration_Start(&hadc1);
}

/*
 * 토양 수분 센서 1회 읽기
 * ADC 변환 시작 → 완료 대기 → 값 읽기 → 정지 순서로 동작
 * 변환 완료 전에 값을 읽으면 이전 변환의 값이 나올 수 있으므로
 * PollForConversion으로 반드시 완료를 확인해야 함
 */
HAL_StatusTypeDef SoilSensor_Read(SoilSensor_Data *data) {
    HAL_ADC_Start(&hadc1);

    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK) {
        HAL_ADC_Stop(&hadc1);
        return HAL_ERROR;
    }

    data->adc_value = HAL_ADC_GetValue(&hadc1);
    data->voltage   = data->adc_value * 3.3f / 4095.0f;

    HAL_ADC_Stop(&hadc1);
    return HAL_OK;
}
