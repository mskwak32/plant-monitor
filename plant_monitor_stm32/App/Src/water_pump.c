#include "water_pump.h"

#include <stdint.h>
#include <stdio.h>


/**
    @param pump_on_ms 펌프 1회 가동시간
    @param soak_ms 펌프 가동 후 토양 흡수 대기 시간
*/
void WaterPump_Init(
    WaterPump_Handle* handle,
    GPIO_TypeDef* port,
    uint16_t pin,
    uint32_t pump_on_ms,
    uint32_t soak_ms) {
    Relay_Init(&handle->relay, port, pin);
    handle->state = WATER_PUMP_IDLE;
    handle->state_start_tick = 0;
    handle->pump_on_ms = pump_on_ms;
    handle->soak_ms = soak_ms;
}

/**
    @param soil_moisture_pct 측정된 토양 수분 퍼센트
    @param threshold 물펌프 가동할 토양 수분 퍼센트
*/
void WaterPump_Run(WaterPump_Handle* handle, uint8_t soil_moisture_pct, uint8_t threshold) {
    uint32_t now = HAL_GetTick();

    switch (handle->state) {
        case WATER_PUMP_IDLE:
            if (soil_moisture_pct < threshold) {
                printf(
                    "[WaterPump] IDLE -> PUMPING (soil=%d%%, thr=%d%%)\r\n", soil_moisture_pct,
                    threshold);
                Relay_Set(&handle->relay, RELAY_ON);
                handle->state = WATER_PUMP_PUMPING;
                handle->state_start_tick = now;
            }
            break;
        case WATER_PUMP_PUMPING:
            if (now - handle->state_start_tick >= handle->pump_on_ms) {
                printf("[WaterPump] PUMPING -> SOAKING\r\n");
                Relay_Set(&handle->relay, RELAY_OFF);
                handle->state = WATER_PUMP_SOAKING;
                handle->state_start_tick = now;
            }
            break;
        case WATER_PUMP_SOAKING:
            if (now - handle->state_start_tick >= handle->soak_ms) {
                printf("[WaterPump] SOAKING -> IDLE\r\n");
                handle->state = WATER_PUMP_IDLE;
            }
            break;
    }
}

WaterPump_State WaterPump_GetState(const WaterPump_Handle* handle) { return handle->state; }