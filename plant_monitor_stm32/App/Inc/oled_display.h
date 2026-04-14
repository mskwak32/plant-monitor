#ifndef APP_INC_OLED_DISPLAY_H_
#define APP_INC_OLED_DISPLAY_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "water_pump.h"

void OledDisplay_Init(I2C_HandleTypeDef *hi2c);
void OledDisplay_Display(
    uint8_t soil_moisture_pct,
    float air_himidity,
    float air_temperature,
    uint8_t soil_threshold,
    WaterPump_State pump_state
);

#endif