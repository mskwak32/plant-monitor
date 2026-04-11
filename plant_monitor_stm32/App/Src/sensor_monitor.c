#include "rht01.h"
#include "sensor_monitor.h"
#include "soil_sensor.h"
#include <stdint.h>
#include <stdio.h>

static RHT01_Handle rht_handle;
static SoilSensor_Handle soil_handle;
static SensorMonitor_Data data;

SensorMonitor_Status SensorMonitor_Init(
  GPIO_TypeDef *air_sensor_gpio_port,
  uint16_t air_sensor_gpio_pin,
  ADC_HandleTypeDef *hadc
) {
  SensorMonitor_Status status;
  
  status.air_sensor =
      RHT01_Init(&rht_handle, air_sensor_gpio_port, air_sensor_gpio_pin);

  if (status.air_sensor != HAL_OK) {
    printf("AirSensor Init Failed: %d\r\n", status.air_sensor);
  } else {
    printf("AirSensor Init OK\r\n");
  }

  status.soil_sensor =
      SoilSensor_Init(&soil_handle, hadc, SOIL_SENSOR_ADC_DRY_DEFAULT,
                      SOIL_SENSOR_ADC_WET_DEFAULT);
  if (status.soil_sensor != HAL_OK) {
    printf("SoilSensor Init Failed: %d\r\n", status.soil_sensor);
  } else {
    printf("SoilSensor Init OK\r\n");
  }

  return status;
}

void SensorMonitor_Run(void) {
  if (SoilSensor_Read(&soil_handle) == HAL_OK) {
    printf("Soil ADC: %lu Voltage: %.2fV\r\n", soil_handle.data.adc_value,
           soil_handle.data.voltage);
    data.soil_moisture_pct = soil_handle.data.moisture_pct;
  } else {
    printf("Soil sensor read failed\r\n");
    data.soil_moisture_pct = 0;
  }

  if(RHT01_Read(&rht_handle) == HAL_OK) {
    printf("Humidity: %.1f%% Temperature: %.1fC\r\n", rht_handle.data.humidity, rht_handle.data.temperature);
    data.air_humidity = rht_handle.data.humidity;
    data.air_temperature = rht_handle.data.temperature;
  } else {
    printf("RHT01 read failed\r\n");
    data.air_humidity = 0.0f;
    data.air_temperature = 0.0f;
  }
}

const SensorMonitor_Data *SensorMonitor_GetData(void) {
    return &data;
}
