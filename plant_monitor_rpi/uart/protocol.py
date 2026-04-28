import json
import logging
from typing import Optional, Union
from models.settings import Settings
from models.sensor_data import SensorData
from models.pump_state import PumpState, PUMP_IDLE, PUMP_PUMPING, PUMP_SOAKING, PUMP_UNKNOWN

# STM32 UART 메시지 필드명
_TYPE_SENSOR_DATA       = "sensor_data"
_TYPE_WATERPUMP         = "water_pump"
_DATA_SOIL_MOISTURE_PCT = "soil_moisture_pct"
_DATA_AIR_TEMPERATURE   = "air_temperature"
_DATA_AIR_HUMIDITY      = "air_humidity"
_DATA_WATERPUMP_STATE   = "state"

# STM32 raw 펌프 상태값 → 정규화 매핑
_PUMP_RAW_IDLE    = "WATER_PUMP_IDLE"
_PUMP_RAW_PUMPING = "WATER_PUMP_PUMPING"
_PUMP_RAW_SOAKING = "WATER_PUMP_SOAKING"

_PUMP_STATE_MAP = {
    _PUMP_RAW_IDLE:    PUMP_IDLE,
    _PUMP_RAW_PUMPING: PUMP_PUMPING,
    _PUMP_RAW_SOAKING: PUMP_SOAKING,
}

logger = logging.getLogger(__name__)

def parse_line(line: str) -> Optional[Union[SensorData, PumpState]]:
    if not line.startswith("msg="):
        return None
    try:
        payload = json.loads(line[4:])
        msg_type = payload.get("type")
        data = payload.get("data", {})
        if msg_type == _TYPE_SENSOR_DATA:
            return SensorData(
                soil_moisture_pct=data.get(_DATA_SOIL_MOISTURE_PCT),
                air_temperature=data.get(_DATA_AIR_TEMPERATURE),
                air_humidity=data.get(_DATA_AIR_HUMIDITY)
            )
        if msg_type == _TYPE_WATERPUMP:
            raw = data.get(_DATA_WATERPUMP_STATE, PUMP_UNKNOWN)
            return PumpState(state=_PUMP_STATE_MAP.get(raw, PUMP_UNKNOWN))
    except (json.JSONDecodeError, KeyError):
        logger.warning("UART 파싱 실패: %s", line)
    return None

def create_setting_message(settings: Settings) -> str:
    return "msg=" + json.dumps({"threshold": settings.threshold}) + "\n"
