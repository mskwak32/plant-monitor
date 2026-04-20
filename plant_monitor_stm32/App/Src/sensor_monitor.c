#include "sensor_monitor.h"
#include <stdint.h>
#include <stdio.h>
#include "rht01.h"
#include "soil_sensor.h"

#define SENSOR_MONITOR_INTERVAL_MS 5000U

static RHT01_Handle rht_handle;
static SoilSensor_Handle soil_handle;
static SensorMonitor_Data data;
static uint32_t last_run_tick = 0;

static void SensorMonitor_SendJson(
    HAL_StatusTypeDef soil_ok,
    HAL_StatusTypeDef rht_ok
);

SensorMonitor_Status SensorMonitor_Init(
    GPIO_TypeDef* air_sensor_gpio_port,
    uint16_t air_sensor_gpio_pin,
    ADC_HandleTypeDef* hadc
) {
    SensorMonitor_Status status;

    status.air_sensor = RHT01_Init(&rht_handle, air_sensor_gpio_port, air_sensor_gpio_pin);

    if (status.air_sensor != HAL_OK) {
        printf("AirSensor Init Failed: %d\r\n", status.air_sensor);
    } else {
        printf("AirSensor Init OK\r\n");
    }

    status.soil_sensor = SoilSensor_Init(
        &soil_handle, hadc, SOIL_SENSOR_ADC_DRY_DEFAULT, SOIL_SENSOR_ADC_WET_DEFAULT);
    if (status.soil_sensor != HAL_OK) {
        printf("SoilSensor Init Failed: %d\r\n", status.soil_sensor);
    } else {
        printf("SoilSensor Init OK\r\n");
    }

    return status;
}

void SensorMonitor_Run(void) {
    uint32_t now = HAL_GetTick();

    // 모니터링 주기 체크
    if (now - last_run_tick < SENSOR_MONITOR_INTERVAL_MS) {
        return;
    }

    last_run_tick = now;
    HAL_StatusTypeDef soil_ok = SoilSensor_Read(&soil_handle);
    HAL_StatusTypeDef rht_ok  = RHT01_Read(&rht_handle);

    if (soil_ok == HAL_OK) {
        data.soil_moisture_pct = soil_handle.data.moisture_pct;
        printf(
            "Soil ADC: %lu Voltage: %.2fV, moisture_pct: %d%%\r\n", soil_handle.data.adc_value,
            soil_handle.data.voltage, data.soil_moisture_pct);
    } else {
        printf("Soil sensor read failed\r\n");
        data.soil_moisture_pct = 0;
    }

    if (rht_ok == HAL_OK) {
        printf(
            "Humidity: %.1f%% Temperature: %.1fC\r\n", rht_handle.data.humidity,
            rht_handle.data.temperature);
        data.air_humidity = rht_handle.data.humidity;
        data.air_temperature = rht_handle.data.temperature;
    } else {
        printf("RHT01 read failed\r\n");
        data.air_humidity = 0.0f;
        data.air_temperature = 0.0f;
    }

    SensorMonitor_SendJson(soil_ok, rht_ok);
}

const SensorMonitor_Data* SensorMonitor_GetData(void) { return &data; }

/**
 * @brief SensorMonitor의 UART 데이터 전송. 전송간 프로토콜은 architecture 문서 참고
 */
static void SensorMonitor_SendJson(
    HAL_StatusTypeDef soil_ok,
    HAL_StatusTypeDef rht_ok
) {
    char buf[128];
    int len = 0;

    len += snprintf(buf + len, sizeof(buf) - len, "msg={\"type\":\"sensor_data\",\"data\":{\"soil_moisture_pct\":");

    if(soil_ok == HAL_OK) {
        len += snprintf(buf + len, sizeof(buf) - len, "%d", data.soil_moisture_pct);
    } else {
        len += snprintf(buf + len, sizeof(buf) - len, "null");
    }

    len += snprintf(buf + len, sizeof(buf) - len, ",\"air_temperature\":");

    if(rht_ok == HAL_OK) {
        len += snprintf(buf + len, sizeof(buf) - len, "%.1f", data.air_temperature);
    } else {
        len += snprintf(buf + len, sizeof(buf) - len, "null");
    }

    len += snprintf(buf + len, sizeof(buf) - len, ",\"air_humidity\":");

    if(rht_ok == HAL_OK) {
        len += snprintf(buf + len, sizeof(buf) - len, "%.1f", data.air_humidity);
    } else {
        len += snprintf(buf + len, sizeof(buf) - len, "null");
    }

    snprintf(buf + len, sizeof(buf) - len, "}}\n");
    printf("%s", buf);
}
