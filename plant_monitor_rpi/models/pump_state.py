from dataclasses import dataclass

# RPi 내부에서 쓰는 정규화된 펌프 상태값
PUMP_IDLE    = "IDLE"
PUMP_PUMPING = "PUMPING"
PUMP_SOAKING = "SOAKING"
PUMP_UNKNOWN = "UNKNOWN"

@dataclass
class PumpState:
    state: str
