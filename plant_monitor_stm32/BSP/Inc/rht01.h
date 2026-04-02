#ifndef INC_RHT01_H_
#define INC_RHT01_H_

#include "main.h"

/*
RHT01에서 읽은 결과를 담는 구조체
*/
typedef struct {
    float humidity;
    float temperature;
} RHT01_Data;

/* 센서 한 번 읽기 */
HAL_StatusTypeDef RHT01_Read(RHT01_Data *data);
/*
센서 5번 읽어서 중간값
RHT01_ReadMedian 내부에서 2초 * 5회 = 약 10초 대기함
*/
HAL_StatusTypeDef RHT01_ReadMedian(RHT01_Data *data);

#endif