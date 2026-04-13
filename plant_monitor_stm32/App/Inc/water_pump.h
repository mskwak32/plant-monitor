#ifndef APP_INC_WATER_PUMP_H_
#define APP_INC_WATER_PUMP_H_

// 펌프 ON 지속 시간과 물 흡수 대기 시간 기본값
#include <stdint.h>
#include "relay.h"
#define WATER_PUMP_ON_MS        3000U   // 3초 동안 펌프 가동
#define WATER_PUMP_SOAK_MS     30000U   // 30초 동안 흡수 대기

typedef enum {
    WATER_PUMP_IDLE,        // 대기 중 - 수분 감시
    WATER_PUMP_PUMPING,     // 펌프 ON - 급수 중
    WATER_PUMP_SOAKING      // 펌프 OFF - 물 흡수 대기
} WaterPump_State;

typedef struct {
    Relay_Handle relay;
    WaterPump_State state;
    uint32_t state_start_tick;      // 현재 상태 진입 시점
    uint32_t pump_on_ms;
    uint32_t soak_ms;
} WaterPump_Handle;

void WaterPump_Init(
    WaterPump_Handle *handle,
    GPIO_TypeDef *port,
    uint16_t pin,
    uint32_t pump_on_ms,
    uint32_t soak_ms
);

void WaterPump_Run(
    WaterPump_Handle *handle,
    uint8_t soil_moisture_pct,
    uint8_t threshold
);

WaterPump_State WaterPump_GetState(const WaterPump_Handle *handle);

#endif