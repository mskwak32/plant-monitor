#ifndef BSP_INC_RELAY_H_
#define BSP_INC_RELAY_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

/*
릴레이 드라이브 (HIGH-active)

HIGH-active: GPIO High -> 릴레이 ON
             GPIO Low  -> 릴레이 OFF
*/

typedef enum {
    RELAY_OFF = 0,
    RELAY_ON = 1
} Relay_State;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    Relay_State state;
} Relay_Handle;

void Relay_Init(Relay_Handle *relay, GPIO_TypeDef *port, uint16_t pin);
void Relay_Set(Relay_Handle *relay, Relay_State state);
Relay_State Relay_Get(Relay_Handle *relay);

#endif