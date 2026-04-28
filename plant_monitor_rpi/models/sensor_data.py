from dataclasses import dataclass
from typing import Optional

@dataclass
class SensorData:
    soil_moisture_pct: Optional[int]
    air_temperature: Optional[float]
    air_humidity: Optional[float]
