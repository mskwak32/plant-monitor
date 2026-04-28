import sqlite3
from pathlib import Path

DB_PATH = Path(__file__).parent.parent / "plant_monitor.db"

def get_connection() -> sqlite3.Connection:
    connection = sqlite3.connect(DB_PATH)
    connection.row_factory = sqlite3.Row
    return connection

def init_db():
    with get_connection() as connection:
        connection.executescript("""
                                 CREATE TABLE IF NOT EXISTS sensor_logs(
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     timestamp TEXT NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%S', 'now')),
                                     soil_moisture_pct INTEGER,
                                     air_humidity REAL,
                                     air_temperature REAL
                                 );
                                 
                                 CREATE TABLE IF NOT EXISTS pump_logs(
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     timestamp TEXT NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%S', 'now')),
                                     action TEXT NOT NULL
                                 );
                                 
                                 CREATE TABLE IF NOT EXISTS settings(
                                     id INTEGER PRIMARY KEY CHECK (id = 1),
                                     threshold INTEGER NOT NULL DEFAULT 30,
                                     updated_at TEXT NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%S', 'now'))
                                 );
                                 
                                 INSERT OR IGNORE INTO settings (id, threshold) VALUES (1, 30);
                                 """)