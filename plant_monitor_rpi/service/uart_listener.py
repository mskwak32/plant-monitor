# uart 포트 연결/재연결 및 데이터 수신 서비스
import asyncio
import logging
import threading
import time
import serial

from db import repository
from uart.protocol import parse_line, get_ping_message, ReadySignal
from models.sensor_data import SensorData
from models.pump_state import PumpState
from uart.serial_port import SerialPort, DEFAULT_PORT
from typing import Optional, Callable

logger = logging.getLogger(__name__)

_RETRY_INTERVAL = 10     # STM32 없을 때 재시도 간격(초)
_PING_RETRY_INTERVAL = 3    # ping 재시도 간격

_current_port: Optional[SerialPort] = None
_on_connected: Optional[Callable] = None        #연결완료 콜백

def get_port() -> Optional[SerialPort]:
    return _current_port

def set_on_connected(callback: Callable) -> None:
    global _on_connected
    _on_connected = callback

def _read_loop(queue: asyncio.Queue,loop: asyncio.AbstractEventLoop) -> None:
    global _current_port
    while True:
        # STM32가 연결될 때까지 반복 시도
        try:
            port = SerialPort()
            _current_port = port
            logger.info("STM32 연결됨 (%s)", port.port_name)
            
            time.sleep(1)
            port.write(get_ping_message())
            logger.info("ping 전송")
        
        except (serial.SerialException, FileNotFoundError):
            logger.info("STM32 없음, %d초 후 재시도...", _RETRY_INTERVAL)
            time.sleep(_RETRY_INTERVAL)
            continue
        
        _synced = False
        _last_ping = time.time()
        
        # 연결된 상태에서 데이터 읽기
        try:
            while True:
                line = port.readline()
                if not line:
                    # stm32에서 발송하는 ready 미수신 상태면 주기적으로 ping 재전송
                    if not _synced and time.time() - _last_ping >= _PING_RETRY_INTERVAL:
                        port.write(get_ping_message())
                        logger.info("ping 재전송")
                        _last_ping = time.time()
                    continue
        
                parsed = parse_line(line)
                if parsed is None:
                    continue
                
                if isinstance(parsed, ReadySignal):
                    logger.info("STM32 ready 수신")
                    _synced = True
                    if _on_connected:
                        try:
                            _on_connected()
                        except Exception:
                            logger.exception("on_connected 콜백 실패")
                    continue
                
                elif isinstance(parsed, SensorData):
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