#include "relay.h"
#include "stm32f1xx_hal_gpio.h"

void Relay_Init(Relay_Handle *relay, GPIO_TypeDef *port, uint16_t pin) {
    relay->port = port;
    relay->pin = pin;
    relay->state = RELAY_OFF;
    HAL_GPIO_WritePin(relay->port, relay->pin, GPIO_PIN_RESET);
}

void Relay_Set(Relay_Handle *relay, Relay_State state) {
    if(state == RELAY_ON) {
        HAL_GPIO_WritePin(relay->port, relay->pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(relay->port, relay->pin, GPIO_PIN_RESET);
    }
    relay->state = state;
}

Relay_State Relay_Get(Relay_Handle *relay) {
    return relay->state;
}