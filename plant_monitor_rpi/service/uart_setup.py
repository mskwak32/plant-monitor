# uart 초기 세팅 동기화 서비스
import logging
from db import repository
from uart import protocol
from service import uart_listener

logger = logging.getLogger(__name__)

# _do_sync 함수를 콜백으로 등록
# STM32 재연결 시마다 _read_loop가 자동으로 _do_sync를 호출
def sync_setting() -> None:
    uart_listener.set_on_connected(_do_sync)
    logger.info("설정값 동기화 콜백 등록 완료")

# UART 세팅 동기화 함수
# uart_listener의 _read_loop의 UART 스레드에서 호출됨
def _do_sync() -> None:
    port = uart_listener.get_port()
    if port is None:
        logger.warning("동기화 시도 시 포트 없음")
        return
    
    settings = repository.get_settings()
    msg = protocol.create_setting_message(settings)
    port.write(msg)
    logger.info("설정값 동기화 완료: threshold=%d%%", settings.threshold)
