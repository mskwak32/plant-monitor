#include "uart_cmd.h"
#include <stdint.h>
#include <string.h>

#define MAX_UART_HANDLES 3

static UartCmd_Handle *handles[MAX_UART_HANDLES];
static uint8_t handle_count = 0;

void UartCmd_Init(UartCmd_Handle *handle, UART_HandleTypeDef *huart) {
    if (handle_count < MAX_UART_HANDLES) {
        handle->huart = huart;
        handle->idx = 0;
        handle->line_ready = 0;
        handles[handle_count++] = handle;

        // UART 수신 인터럽트 활성화
        HAL_UART_Receive_IT(handle->huart, &handle->rx_byte, 1);
    }
}

uint8_t UartCmd_HasLine(UartCmd_Handle *handle) {
    return handle->line_ready;
}

void UartCmd_GetLine(UartCmd_Handle *handle, char *out) {
        strcpy(out, handle->line_buf);
        handle->line_ready = 0;
}

/**
 * HAL weak 심볼 override — 프로젝트 전체에 하나만 존재해야 함
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    for(uint8_t i = 0; i < handle_count; i++) {
        UartCmd_Handle *h = handles[i];
        if(h->huart->Instance != huart->Instance) continue;

        // 수신한 바이트가 \n이면 line_ready 플래그 세우고 idx 초기화
        if(h->rx_byte == '\n') {
            h->line_buf[h->idx] = '\0';  // 문자열 종료
            h->line_ready = 1;           // 플래그만 세팅, 처리는 메인 루프에서
            h->idx = 0;
        } else if(h->rx_byte != '\r' && h->idx < UART_CMD_BUF_SIZE - 1) {   
            // \r은 무시, 버퍼가 넘치지 않도록 체크
            h->line_buf[h->idx++] = h->rx_byte;
        }

        // 다음 바이트 수신 대기
        HAL_UART_Receive_IT(h->huart, &h->rx_byte, 1);
        break;
    }
}