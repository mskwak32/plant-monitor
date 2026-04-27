import json
import logging
from dataclasses import dataclass
from typing import Optional, Union
from models.settings import Settings

TYPE_SENSOR_DATA = "sensor_data"
TYPE_WATERPUMP = "water_pump"
DATA_SOIL_MOISTURE_PCT = "soil_moisture_pct"
DATA_AIR_TEMPERATURE = "air_temperature"
DATA_AIR_HUMIDITY = "air_humidity"
DATA_WATERPUMP_STATE = "state"
WATERPUMP_STATE_UNKNOWN = "UNKNOWN"

logger = logging.getLogger(__name__)

@dataclass
class SensorData:
    soil_moisture_pct: Optional[int]
    air_temperature: Optional[float]
    air_humidity: Optional[float]
    
@dataclass
class PumpState:
    state: str
    
def parse_line(line: str) -> Optional[Union[SensorData, PumpState]]:
    if not line.startswith("msg="):
        return None
    try:
        payload = json.loads(line[4:])
        msg_type = payload.get("type")
        data = payload.get("data", {})
        if msg_type == TYPE_SENSOR_DATA:
            return SensorData(
                soil_moisture_pct=data.get(DATA_SOIL_MOISTURE_PCT),
                air_temperature=data.get(DATA_AIR_TEMPERATURE),
                air_humidity=data.get(DATA_AIR_HUMIDITY)
            )
        if msg_type == TYPE_WATERPUMP:
            return PumpState(state=data.get(DATA_WATERPUMP_STATE, WATERPUMP_STATE_UNKNOWN))
    except (json.JSONDecodeError, KeyError):
        logger.warning("UART 파싱 실패: %s", line)
    return None

def create_setting_message(settings: Settings) -> str:
    return "msg=" + json.dumps({"threshold": settings.soil_moisture_min}) + "\n"