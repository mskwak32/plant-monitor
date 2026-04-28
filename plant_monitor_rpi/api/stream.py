# SSE 엔드포인트
import asyncio
import json
from dataclasses import asdict
from fastapi import APIRouter, Request
from fastapi.responses import StreamingResponse
from models.sensor_data import SensorData
from models.pump_state import PumpState
from api.constants import TYPE_KEY, SENSOR_DATA_TYPE, PUMP_STATE_TYPE

router = APIRouter()

async def _event_generator(request: Request, queue: asyncio.Queue):
    while True:
        if await request.is_disconnected():
            break

        try:
            # 데이터가 없을 때 무한 블로킹하면 이벤트 루프 전체가 멈추므로 1초 타임아웃
            parsed = await asyncio.wait_for(queue.get(), timeout=1.0)
        except asyncio.TimeoutError:
            # 데이터 없음 → keepalive 코멘트 전송 (TCP idle 타임아웃 방지)
            yield ": keepalive\n\n"
            continue

        if isinstance(parsed, SensorData):
            payload = {TYPE_KEY: SENSOR_DATA_TYPE, **asdict(parsed)}
        elif isinstance(parsed, PumpState):
            payload = {TYPE_KEY: PUMP_STATE_TYPE, **asdict(parsed)}
        else:
            continue
        
        # SSE 프로토콜: "data: <내용>\n\n" 형식. \n\n 이 메시지 경계
        yield f"data: {json.dumps(payload)}\n\n"
        
@router.get("/stream")
async def stream(request: Request):
    queue: asyncio.Queue = request.app.state.queue
    return StreamingResponse(
        _event_generator(request, queue),
        media_type="text/event-stream",
        headers={
            "Cache-Control": "no-cache",
            "X-Accel-Buffering": "no"       # nginx 버퍼링 끄기. 버퍼링하면 실시간성이 깨짐
        }
    )
        
        