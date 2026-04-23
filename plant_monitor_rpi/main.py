from contextlib import asynccontextmanager
import asyncio
import logging

from fastapi import FastAPI
from uart.serial_port import SerialPort
from service import uart_listener
from db.database import init_db

logging.basicConfig(level=logging.INFO)

@asynccontextmanager
async def lifespan(app: FastAPI):
    init_db()
    queue: asyncio.Queue = asyncio.Queue()
    uart_listener.start(queue)  # 포트 관리는 리스너가 담당
    
    # 엔드포인트에서 request.app.state.queue 로 접근할 수 있도록 app.state에 queue세팅
    app.state.queue = queue
    
    yield
    
app = FastAPI(lifespan=lifespan)

from api import stream
app.include_router(stream.router)