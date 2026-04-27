# uart 초기 세팅 동기화 서비스
import logging
from db import repository
from uart import protocol
from service import uart_listener

logger = logging.getLogger(__name__)

def sync_setting() -> None:
    logger.info("STM32 연결 대기 중(초기 설정값 동기화)...")
    uart_listener.wait_until_connected()        # 연결되기까지 스레드 대기

    settings = repository.get_settings()
    msg = protocol.create_setting_message(settings)
    uart_listener.get_port().write(msg)

    logger.info("초기 설정값 동기화 완료: soil_moisture_min=%d%%", settings.soil_moisture_min)
