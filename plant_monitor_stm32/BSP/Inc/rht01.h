#ifndef BSP_INC_RHT01_H_
#define BSP_INC_RHT01_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

/*
온습도센서(RHT01) 드라이버
RHT01에서 읽은 결과를 담는 구조체
*/
typedef struct {
    float humidity;
    float temperature;
} RHT01_Data;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    RHT01_Data data;
} RHT01_Handle;

void RHT01_Init(RHT01_Handle *handle, GPIO_TypeDef *port, uint16_t pin);
HAL_StatusTypeDef RHT01_Read(RHT01_Handle *handle);
HAL_StatusTypeDef RHT01_ReadMedian(RHT01_Handle *handle);

#endif