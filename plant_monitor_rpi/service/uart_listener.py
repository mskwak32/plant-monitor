# uart 포트 연결/재연결 및 데이터 수신 서비스
import asyncio
import logging
import threading
import time
import serial

from db import repository
from uart.protocol import parse_line
from models.sensor_data import SensorData
from models.pump_state import PumpState
from uart.serial_port import SerialPort, DEFAULT_PORT
from typing import Optional

logger = logging.getLogger(__name__)

_RETRY_INTERVAL = 10     # STM32 없을 때 재시도 간격(초)

_current_port: Optional[SerialPort] = None
_connected_event = threading.Event()        # 연결 상태 이벤트

def get_port() -> Optional[SerialPort]:
    return _current_port

def wait_until_connected(timeout: float = None) -> bool:
    return _connected_event.wait(timeout)

def _read_loop(queue: asyncio.Queue,loop: asyncio.AbstractEventLoop) -> None:
    global _current_port
    while True:
        # STM32가 연결될 때까지 반복 시도
        try:
            port = SerialPort()
            _current_port = port
            _connected_event.set()      # 연결 이벤트 발행
            logger.info("STM32 연결됨 (%s)", port.port_name)
        except (serial.SerialException, FileNotFoundError):
            logger.info("STM32 없음, %d초 후 재시도...", _RETRY_INTERVAL)
            time.sleep(_RETRY_INTERVAL)
            continue
        
        # 연결된 상태에서 데이터 읽기
        try:
            while True:
                line = port.readline()
                if not line:
                    continue
        
                parsed = parse_line(line)
                if parsed is None:
                    continue
        
                if isinstance(parsed, SensorData):
                    try:
                        repository.insert_sensor(parsed)
                    except Exception:
                        logger.exception("sensor DB 저장 실패")
                elif isinstance(parsed, PumpState):
                    try:
                        repository.insert_pump(parsed)
                    except Exception:
                        logger.exception("pump DB 저장 실패")
        
                loop.call_soon_threadsafe(queue.put_nowait, parsed)
        except (serial.SerialException, OSError):
            # STM32 분리됨 -> 바깥 while로 돌아가 재연결 시도
            _current_port = None
            _connected_event.clear()    # 연결 끊김 이벤트 발행
            logger.warning("STM32 연결 끊김, 재연결 대기...")
            port.close()
        
def start(queue: asyncio.Queue) -> None:
    loop = asyncio.get_event_loop()
    thread = threading.Thread(
        target=_read_loop,
        args=(queue, loop),
        daemon=True,
        name="uart-listener"
    )
    thread.start()
    logger.info("UART 리스너 시작 (port=%s)", DEFAULT_PORT)