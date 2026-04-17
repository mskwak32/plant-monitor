#ifndef BSP_INC_UART_CMD_H_
#define BSP_INC_UART_CMD_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define UART_CMD_BUF_SIZE 32

typedef struct {
    UART_HandleTypeDef *huart;
    uint8_t rx_byte;                    // HAL이 수신한 바이트를 1바이트씩 써주는 버퍼
    char line_buf[UART_CMD_BUF_SIZE];   // \n 전까지 누적하는 라인 버퍼
    uint8_t idx;                        // line_buf의 현재 쓰기 위치
    volatile uint8_t line_ready;        // ISR과 메인 루프가 공유 → volatile 필수
} UartCmd_Handle;

void UartCmd_Init(UartCmd_Handle *handle, UART_HandleTypeDef *huart);
uint8_t UartCmd_HasLine(UartCmd_Handle *handle);
void UartCmd_GetLine(UartCmd_Handle *handle, char *out);

#endif