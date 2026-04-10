#include "soil_sensor.h"

/**
 초기화 및 ADC 보정(캘리브레이션) 실행
 STM32F1 ADC는 내부 오프셋 오차가 있어서, 최초 1회 보정이 필요함.
 보정하지 않으면 측정값에 최대 ±2LSB 오차가 생길 수 있음.
 @param adc_dry 건조할 때 ADC raw value
 @param adc_wet 완전 젖었을 때 ADC raw value
 */
HAL_StatusTypeDef SoilSensor_Init(
    SoilSensor_Handle *handle,
    ADC_HandleTypeDef *hadc, 
    uint16_t adc_dry,
    uint16_t adc_wet
) {
  handle->hadc = hadc;
  handle->adc_dry = adc_dry;
  handle->adc_wet = adc_wet;
  return HAL_ADCEx_Calibration_Start(handle->hadc);
}

/*
 * 토양 수분 센서 1회 읽기
 * ADC 변환 시작 → 완료 대기 → 값 읽기 → 정지 순서로 동작
 */
HAL_StatusTypeDef SoilSensor_Read(SoilSensor_Handle *handle) {
  HAL_ADC_Start(handle->hadc);

  // 변환 완료 전에 값을 읽으면 이전 변환의 값이 나올 수 있으므로
  // PollForConversion으로 반드시 완료를 확인해야 함
  if (HAL_ADC_PollForConversion(handle->hadc, HAL_MAX_DELAY) != HAL_OK) {
    HAL_ADC_Stop(handle->hadc);
    return HAL_ERROR;
  }

  handle->data.adc_value = HAL_ADC_GetValue(handle->hadc);
  handle->data.voltage = handle->data.adc_value * 3.3f / (float)SOIL_SENSOR_ADC_MAX;

  HAL_ADC_Stop(handle->hadc);
  return HAL_OK;
}
