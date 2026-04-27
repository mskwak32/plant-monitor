from contextlib import asynccontextmanager
import asyncio
import logging

from fastapi import FastAPI
from service import uart_listener, uart_setup
from db.database import init_db
from api import stream, sensor, pump, settings

logging.basicConfig(level=logging.INFO)

@asynccontextmanager
async def lifespan(app: FastAPI):
    init_db()
    queue: asyncio.Queue = asyncio.Queue()
    uart_listener.start(queue)

    loop = asyncio.get_event_loop()
    loop.run_in_executor(None, uart_setup.sync_setting)

    app.state.queue = queue
    
    yield
    
app = FastAPI(lifespan=lifespan)
app.include_router(stream.router)
app.include_router(sensor.router)
app.include_router(pump.router)
app.include_router(settings.router)