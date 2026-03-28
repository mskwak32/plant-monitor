# 포트폴리오 프로젝트

> 스마트 화분 관리 시스템 (Smart Plant Monitoring System)
> 안드로이드 개발자 → IoT 개발자 전향 포트폴리오

---

**스마트 화분 모니터링 시스템**

### 시스템 구성

```
[토양습도센서] ──ADC──▶ [STM32 Nucleo F103RB] ◀── [DHT11]
                                  │
                            UART 시리얼
                                  │
                                  ▼
                        [라즈베리파이5 8GB]
                        + OLED 디스플레이
                                  │
                            Wi-Fi 내부망
                                  │
                                  ▼
                        [안드로이드 앱 (픽셀폰)]
```

### 레이어별 역할

| 레이어 | 담당 | 핵심 기능 |
|---|---|---|
| 펌웨어 | STM32 | 센서 읽기(ADC/DHT11), 임계값 기반 펌프 자동 제어, UART 송수신 / C, CubeIDE |
| 백엔드 | 라즈베리파이5 | UART 수신, SQLite 저장, FastAPI REST API, JWT 인증, OLED 표시 / Python |
| 프론트 | 안드로이드 앱 | 실시간 조회, 그래프, 펌프 이력, 습도 임계값 설정 / Kotlin, Retrofit, MPAndroidChart |
| 네트워크 | Wi-Fi 내부망 | 완성 후 여유 있으면 ngrok 외부망 추가 |

### REST API

| Method | Endpoint | 설명 |
|---|---|---|
| POST | /auth/login | JWT 로그인 |
| GET | /sensor/current | 현재 센서값 |
| GET | /sensor/logs | 시간별 로그 |
| GET | /pump/logs | 펌프 이력 |
| POST | /settings/threshold | 임계값 설정 |
| GET | /settings/threshold | 임계값 조회 |

### DB 설계

**sensor_logs**: id, timestamp, soil_humidity, air_humidity, temperature

**pump_logs**: id, timestamp, action(ON/OFF), trigger(AUTO/MANUAL)

**settings**: id, soil_humidity_min, updated_at

---

## 부품 목록

| 부품 | 모델/규격 |
|---|---|
| MCU | STM32 Nucleo F103RB |
| 토양습도센서 | SZH-EK106 |
| 온습도센서 | DHT11 |
| 릴레이 모듈 | 5V 4채널 / LOW 신호에 ON되는 방식 확인 필요 |
| 워터펌프 | DC12V 수중펌프 (외경8mm 내경6mm) |
| 실리콘 호스 | 외경10mm 내경8mm |
| 라즈베리파이5 | 8GB |
| 라즈베리파이 어댑터 | 5V/5A USB-C 공식 |
| OLED 디스플레이 | 0.96인치 I2C SSD1306 / 라즈베리파이에 연결 |
| 12V DC 어댑터 | 5.5mm/2.1mm 센터플러스 2A / 펌프 전원용 |
| 브레드보드 | 830핀 MB-102 |
| SD카드 | 64GB |
| 점퍼 와이어 | 수-수, 수-암 |

---

## 전원 구성

| 기기 | 전원 |
|---|---|
| 라즈베리파이5 | 공식 어댑터 5V/5A USB-C |
| STM32 Nucleo | 라즈베리파이5 USB 포트 연결 (전원 + UART 겸용) |
| 펌프 + 릴레이 | 12V DC 어댑터 별도 공급 |

---

## 개발 일정

| 기간 | 레이어 | 목표 |
|---|---|---|
| 1~2주 | STM32 | CubeIDE 세팅, LED Blink, UART로 PC에 "Hello" 출력 |
| 3~4주 | STM32 | 토양센서 ADC 읽기, DHT11 읽기, UART 송신 |
| 5주 | STM32 | 릴레이로 펌프 GPIO 제어, 임계값 기반 자동 제어, UART 명령 수신 |
| 6~7주 | 라즈베리파이 | OS 세팅, UART 수신, SQLite DB 저장, OLED 표시 |
| 8주 | 라즈베리파이 | FastAPI REST API, JWT 인증 |
| 9~10주 | 안드로이드 앱 | UI, API 연동, 그래프 |
| 11주 | 전체 | 통합 테스트, 버그 수정 |
| 12주 | 마무리 | README, GitHub, 시연 영상 |

---
