#include "rht01.h"

/*
 * GPIO를 Output 모드로 전환.
 * Start Signal 전송 시 MCU가 라인을 직접 제어해야함
 */
static void Set_Output(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = RHT01_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RHT01_GPIO_Port, &GPIO_InitStruct);
}

/*
 *	GPIO를 Input 모드로 전환(내부 풀업)
 *	데이터 수신시 센서가 라인을 제어
 */
static void Set_Input(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = RHT01_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(RHT01_GPIO_Port, &GPIO_InitStruct);
}

/*
 * micro second(us)단위의 딜레이
 * HAL_Delay()는 ms 단위라 RHT01 타이밍(수십us)에 쓸 수 없음
 * 		DWT->CYCCNT: Cpu 클럭마다 1씩 증가하는 하드웨어 카운터
 * 		72MHz -> 1us = 72 ticks
 */
static void delay_us(uint32_t us) {
  uint32_t start = DWT->CYCCNT;
  uint32_t ticks = us * (SystemCoreClock / 1000000U);
  // 뺄셈방식 : CYCCNT가 overflow 되어도 올바르게 동작
  while ((DWT->CYCCNT - start) < ticks)
    ;
}

/*
 * RHT01 1회 읽기
 */
HAL_StatusTypeDef RHT01_Read(RHT01_Data *data) {
  uint8_t bits[40] = {0};
  uint32_t timeout;

  // Step 1: Start signal
  // 5ms LOW -> 30us HIGH
  Set_Output();
  HAL_GPIO_WritePin(RHT01_GPIO_Port, RHT01_Pin, GPIO_PIN_RESET);
  HAL_Delay(5);

  HAL_GPIO_WritePin(RHT01_GPIO_Port, RHT01_Pin, GPIO_PIN_SET);
  delay_us(30);

  // Step 2: Check sensor response
  // 센서가 80us LOW -> 80us HIGH로 응답
  // timeout: 센서 고장/미연결 시 무한 루프 방지
  Set_Input();

  // LOW 기다림
  timeout = 10000;
  while (HAL_GPIO_ReadPin(RHT01_GPIO_Port, RHT01_Pin) == GPIO_PIN_SET) {
    if (--timeout == 0)
      return HAL_ERROR;
  }

  // LOW 구간(80us) 끝나길 기다림
  timeout = 10000;
  while (HAL_GPIO_ReadPin(RHT01_GPIO_Port, RHT01_Pin) == GPIO_PIN_RESET) {
    if (--timeout == 0)
      return HAL_ERROR;
  }

  // HIGH 구간(80us) 끝나길 기다림 -> 이후 데이터 전송 시작
  timeout = 10000;
  while (HAL_GPIO_ReadPin(RHT01_GPIO_Port, RHT01_Pin) == GPIO_PIN_SET) {
    if (--timeout == 0)
      return HAL_ERROR;
  }

  // Step 3: 40bits 데이터 수신
  // 각 비트 구조:
  //		50us LOW -> HIGH (26~28us = "0" / 70us = "1")
  // 판단: LOW 끝난 후 40us 뒤에 라인이 아직 HIGH이면 "1","LOW"이면 "0"
  for (int i = 0; i < 40; i++) {
    // 50us LOW 구간이 끝나길 기다림(매 비트 공통)
    timeout = 10000;
    while (HAL_GPIO_ReadPin(RHT01_GPIO_Port, RHT01_Pin) == GPIO_PIN_RESET) {
      if (--timeout == 0)
        return HAL_ERROR;
    }

    // 40us 후 라인 상태로 비트 판별
    delay_us(40);
    bits[i] =
        (HAL_GPIO_ReadPin(RHT01_GPIO_Port, RHT01_Pin) == GPIO_PIN_SET) ? 1 : 0;

    // HIGH 구간이 끝나길 기다려야 다음 비트 시작 가능
    timeout = 10000;
    while (HAL_GPIO_ReadPin(RHT01_GPIO_Port, RHT01_Pin) == GPIO_PIN_SET) {
      if (--timeout == 0)
        return HAL_ERROR;
    }
  }

  // Step 4: 40bits -> 5bytes로 변환
  uint8_t bytes[5] = {0};
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 8; j++) {
      bytes[i] = (bytes[i] << 1) | bits[i * 8 + j];
    }
  }

  // Step 5: checksum 검증
  uint8_t checksum = bytes[0] + bytes[1] + bytes[2] + bytes[3];
  if (checksum != bytes[4])
    return HAL_ERROR;

  // Step 6: 데이터 변환
  data->humidity = bytes[0] + bytes[1] * 0.1f;
  // 온도 MSB(최상위비트)가 1이면 영하
  if (bytes[2] & 0x80) {
    data->temperature = -((bytes[2] & 0x7F) + bytes[3] * 0.1f);
  } else {
    data->temperature = bytes[2] + bytes[3] * 0.1f;
  }

  return HAL_OK;
}

/*
5회 읽어서 중간값(median) 변환
노이즈/노화로 인한 이상값 최소화
*/
HAL_StatusTypeDef RHT01_ReadMedian(RHT01_Data *data) {
  float hum[5], temp[5];
  int valid = 0;

  for (int i = 0; i < 5; i++) {
    RHT01_Data d;
    if (RHT01_Read(&d) == HAL_OK) {
      hum[valid] = d.humidity;
      temp[valid] = d.temperature;
      valid++;
    }

    // 데이터시트: 측정 간격 2초 이상 필요
    HAL_Delay(2000);
  }

  // 한 번도 성공하지 못하면 실패 반환
  if (valid == 0)
    return HAL_ERROR;

  // 버블 정렬 후 중간값 선택
  for (int i = 0; i < valid - 1; i++) {
    for (int j = 0; j < valid - i - 1; j++) {
      if (hum[j] > hum[j + 1]) {
        float t = hum[j];
        hum[j] = hum[j + 1];
        hum[j + 1] = t;
        t = temp[j];
        temp[j] = temp[j + 1];
        temp[j + 1] = t;
      }
    }
  }

  data->humidity = hum[valid / 2];
  data->temperature = temp[valid / 2];

  return HAL_OK;
}
