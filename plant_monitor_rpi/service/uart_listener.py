import asyncio
import logging
import threading

from db import repository
from uart.protocol import parse_line, SensorData, PumpState
from uart.serial_port import SerialPort

logger = logging.getLogger(__name__)

def _read_loop(
    port: SerialPort,
    queue: asyncio.Queue,
    loop: asyncio.AbstractEventLoop
) -> None:
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
        
def start(port: SerialPort, queue: asyncio.Queue) -> None:
    loop = asyncio.get_event_loop()
    thread = threading.Thread(
        target=_read_loop,
        args=(port, queue, loop),
        daemon=True,
        name="uart-listener"
    )
    thread.start()
    logger.info("UART 리스너 시작 (port=%s)", port._ser.port)