import sqlite3
from typing import Optional
from db.database import get_connection
from uart.protocol import SensorData, PumpState

def insert_sensor(data: SensorData) -> None:
    with get_connection() as connection:
        connection.execute(
            "INSERT INTO sensor_logs (soil_moisture_pct, air_humidity, air_temperature) "
            "VALUES (?, ?, ?)",
             (data.soil_moisture_pct, data.air_humidity, data.air_temperature)
        )
        
def get_latest_sensor() -> Optional[sqlite3.Row]:
    with get_connection() as connection:
        return connection.execute(
            "SELECT * FROM sensor_logs ORDER BY id DESC LIMIT 1"
        ).fetchone()
        
def get_sensor_history(limit: int = 100) -> list[sqlite3.Row]:
    with get_connection() as connection:
        return connection.execute(
            "SELECT * FROM sensor_logs ORDER BY id DESC LIMIT ?", (limit,)
        ).fetchall()
        
def insert_pump(pump: PumpState) -> None:
    with get_connection() as connection:
        connection.execute(
            "INSERT INTO pump_logs (action) VALUES (?)", (pump.state,)
        )

def get_settings() -> Optional[sqlite3.Row]:
    with get_connection() as connection:
        return connection.execute("SELECT * FROM settings WHERE id = 1").fetchone()
    
def update_soil_min(value: int) -> None:
    with get_connection() as connection:
        connection.execute(
            "UPDATE settings SET soil_moisture_min = ?, "
            "updated_at = strftime('%Y-%m-%dT%H:%M:%S', 'now') WHERE id = 1",
            (value,)
        )