#include "plant_monitor.h"
#include "relay.h"
#include "rht01.h"
#include "soil_sensor.h"
#include "ssd1306.h"
#include <stdio.h>

static RHT01_Handle rht_handle;
static SoilSensor_Handle soil_handle;
static SSD1306_Handle oled_handle;
static uint8_t oled_i2c_addr = 0x78;

static void OLED_Display(uint8_t soil_moisture_ptc, RHT01_Data *rht, uint8_t soil_threshold);

void PlantMonitor_Init(
  GPIO_TypeDef *rht_gpio_port,
  uint16_t rht_gpio_pin,
  I2C_HandleTypeDef *hi2c,
  ADC_HandleTypeDef *hadc
) {
  RHT01_Init(&rht_handle, rht_gpio_port, rht_gpio_pin);

  HAL_StatusTypeDef soil_status =SoilSensor_Init(&soil_handle, hadc, SOIL_SENSOR_ADC_DRY_DEFAULT, SOIL_SENSOR_ADC_WET_DEFAULT);

  if (soil_status != HAL_OK) {
    printf("SoilSensor Init Failed: %d\r\n", soil_status);
  } else {
    printf("SoilSensor Init OK\r\n");
  }

  HAL_StatusTypeDef oled_status =
      SSD1306_Init(&oled_handle, hi2c, oled_i2c_addr);

  if (oled_status != HAL_OK) {
    printf("SSD1306(Displany) Init Failed: %d\r\n", oled_status);
  } else {
    printf("SSD1306(Display) Init OK\r\n");
  }
}

void PlantMonitor_Run(uint8_t soil_threshold) {
  uint8_t moisture_pct; // 토양습도 퍼센트

  if (SoilSensor_Read(&soil_handle) == HAL_OK) {
    printf("Soil ADC: %lu Voltage: %.2fV\r\n", soil_handle.data.adc_value,soil_handle.data.voltage);

    if (soil_handle.data.adc_value >= soil_handle.adc_dry) {
      moisture_pct = 0;
    } else if (soil_handle.data.adc_value <= soil_handle.adc_wet) {
      moisture_pct = 100;
    } else {
      moisture_pct =(uint8_t)((soil_handle.adc_dry - soil_handle.data.adc_value) * 100 / (soil_handle.adc_dry - soil_handle.adc_wet));
    }
  } else {
    printf("Soil sensor real failed\r\n");
    moisture_pct = 0;
  }

  if (RHT01_Read(&rht_handle) == HAL_OK) {
    printf("Humidity: %.1f%% Temperature: %.1fC\r\n", rht_handle.data.humidity,rht_handle.data.temperature);
  } else {
    printf("RHT01 read failed\r\n");
    rht_handle.data.temperature = 0.0f;
    rht_handle.data.humidity = 0.0f;
  }

  OLED_Display(moisture_pct, &rht_handle.data, soil_threshold);
}

static void OLED_Display(uint8_t soil_moisture_ptc, RHT01_Data *rht, uint8_t soil_threshold) {
  char oled_buf[24];

  SSD1306_Clear(&oled_handle, SSD1306_BLACK);

  sprintf(oled_buf, "Soil: %13d%%", soil_moisture_ptc);
  SSD1306_WriteString(&oled_handle,0, 0, oled_buf);

  sprintf(oled_buf, "Threashold: %7d%%", soil_threshold);
  SSD1306_WriteString(&oled_handle,0, 1, oled_buf);

  sprintf(oled_buf, "Temp:%14.1fC", rht->temperature);
  SSD1306_WriteString(&oled_handle, 0, 3, oled_buf);

  sprintf(oled_buf, "Humidity:%10.1f%%", rht->humidity);
  SSD1306_WriteString(&oled_handle, 0, 4, oled_buf);

  SSD1306_WriteString(&oled_handle, 0, 6, "Pump: OFF");
  SSD1306_Update(&oled_handle);
}