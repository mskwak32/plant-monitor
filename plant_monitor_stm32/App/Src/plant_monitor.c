#include "plant_monitor.h"
#include "rht01.h"
#include "soil_sensor.h"
#include "ssd1306.h"
#include "stm32f1xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>


// 물펌프를 작동시킬 토양수분 임계값
static uint8_t soil_threashold = 40;

static void OLED_Display(uint8_t soil_moisture_ptc, RHT01_Data *rht);

void PlantMonitor_Init(I2C_HandleTypeDef *hi2c, ADC_HandleTypeDef *hadc) {
  HAL_StatusTypeDef soil_status = SoilSensor_Init(hadc);
  if (soil_status != HAL_OK) {
    printf("SoilSensor Init Failed: %d\r\n", soil_status);
  } else {
    printf("SoilSensor Init OK\r\n");
  }

  HAL_StatusTypeDef oled_status = SSD1306_Init(hi2c);
  if (oled_status != HAL_OK) {
    printf("SSD1306(Displany) Init Failed: %d\r\n", oled_status);
  } else {
    printf("SSD1306(Display) Init OK\r\n");
  }
}

void PlantMonitor_Run(void) {
  RHT01_Data rht;
  SoilSensor_Data soil;
  uint8_t moisture_pct; // 토양습도 퍼센트

  if (SoilSensor_Read(&soil) == HAL_OK) {
    printf("Soil ADC: %lu Voltage: %.2fV\r\n", soil.adc_value, soil.voltage);

    if(soil.adc_value >= SOIL_SENSOR_ADC_DRY) {
        moisture_pct = 0;
    } else if(soil.adc_value <= SOIL_SENSOR_ADC_WET) {
        moisture_pct = 100;
    } else {
        moisture_pct = (uint8_t)((SOIL_SENSOR_ADC_DRY - soil.adc_value) * 100 / (SOIL_SENSOR_ADC_DRY - SOIL_SENSOR_ADC_WET));
    }
  } else {
    printf("Soil sensor real failed\r\n");
    moisture_pct = 0;
  }

  if (RHT01_Read(&rht) == HAL_OK) {
    printf("Humidity: %.1f%% Temperature: %.1fC\r\n", rht.humidity,
           rht.temperature);
  } else {
    printf("RHT01 read failed\r\n");
    rht.temperature = 0.0f;
    rht.humidity = 0.0f;
  }

  OLED_Display(moisture_pct, &rht);
}

static void OLED_Display(uint8_t soil_moisture_ptc, RHT01_Data *rht) {
  char oled_buf[24];

  SSD1306_Clear(SSD1306_BLACK);

  sprintf(oled_buf, "Soil: %3d%%", soil_moisture_ptc);
  SSD1306_WriteString(0, 0, oled_buf);

  sprintf(oled_buf, "Threashold: %3d%%", soil_threashold);
  SSD1306_WriteString(0, 1, oled_buf);

  sprintf(oled_buf, "Temp:%5.1fC", rht->temperature);
  SSD1306_WriteString(0, 2, oled_buf);

  sprintf(oled_buf, "Humidity:%5.1f%%", rht->humidity);
  SSD1306_WriteString(0, 3, oled_buf);

  SSD1306_WriteString(0, 4, "Pump: OFF");
  SSD1306_Update();
}