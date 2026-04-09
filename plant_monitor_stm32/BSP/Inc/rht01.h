#ifndef BSP_INC_RHT01_H_
#define BSP_INC_RHT01_H_

#include "main.h"

/*
온습도센서(RHT01) 드라이버
RHT01에서 읽은 결과를 담는 구조체
*/
typedef struct {
    float humidity;
    float temperature;
} RHT01_Data;

HAL_StatusTypeDef RHT01_Read(RHT01_Data *data);
HAL_StatusTypeDef RHT01_ReadMedian(RHT01_Data *data);

#endif