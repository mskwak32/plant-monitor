#include "plant_monitor.h"
#include "sensor_monitor.h"
#include "ssd1306.h"
#include <stdio.h>

static SSD1306_Handle oled_handle;
static uint8_t oled_i2c_addr = 0x78;

static void OLED_Display(
  uint8_t soil_moisture_ptc, 
  float air_humidity,
  float air_temperature,
  uint8_t soil_threshold);

void PlantMonitor_Init(
  GPIO_TypeDef *air_sensor_gpio_port,
  uint16_t air_sensor_gpio_pin, 
  I2C_HandleTypeDef *hi2c,
  ADC_HandleTypeDef *hadc
) {
  SensorMonitor_Init(air_sensor_gpio_port, air_sensor_gpio_pin, hadc);

  HAL_StatusTypeDef oled_status =
      SSD1306_Init(&oled_handle, hi2c, oled_i2c_addr);

  if (oled_status != HAL_OK) {
    printf("SSD1306(Display) Init Failed: %d\r\n", oled_status);
  } else {
    printf("SSD1306(Display) Init OK\r\n");
  }
}

void PlantMonitor_Run(uint8_t soil_threshold) {
  SensorMonitor_Run();
  const SensorMonitor_Data *data = SensorMonitor_GetData();

  OLED_Display(
    data->soil_moisture_pct,
    data->air_humidity,
    data->air_temperature,
    soil_threshold
    );
}

static void OLED_Display(
  uint8_t soil_moisture_ptc,
  float air_humidity,
  float air_temperature,
  uint8_t soil_threshold
) {
  char oled_buf[24];

  SSD1306_Clear(&oled_handle, SSD1306_BLACK);

  sprintf(oled_buf, "Soil: %13d%%", soil_moisture_ptc);
  SSD1306_WriteString(&oled_handle, 0, 0, oled_buf);

  sprintf(oled_buf, "Threshold: %7d%%", soil_threshold);
  SSD1306_WriteString(&oled_handle, 0, 1, oled_buf);

  sprintf(oled_buf, "Temp:%14.1fC", air_temperature);
  SSD1306_WriteString(&oled_handle, 0, 3, oled_buf);

  sprintf(oled_buf, "Humidity:%10.1f%%", air_humidity);
  SSD1306_WriteString(&oled_handle, 0, 4, oled_buf);

  SSD1306_WriteString(&oled_handle, 0, 6, "Pump: OFF");
  SSD1306_Update(&oled_handle);
}