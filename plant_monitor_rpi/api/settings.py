# 설정 API 라우터
from dataclasses import asdict
from fastapi import APIRouter, HTTPException
from pydantic import BaseModel, Field
from uart import protocol

from db import repository
from service import uart_listener

router = APIRouter()

class SettingsResponse(BaseModel):
    threshold: int
    updated_at: str

class ThresholdRequest(BaseModel):
    threshold: int = Field(ge=0, le=100)
    
@router.get("/settings", response_model=SettingsResponse)
async def get_settings():
    return asdict(repository.get_settings())

@router.post("/settings/threshold", response_model=SettingsResponse)
async def update_threshold(body: ThresholdRequest):
    port = uart_listener.get_port()
    if port is None:
        raise HTTPException(status_code=503, detail="STM32 미연결")

    repository.update_threshold(body.threshold)

    msg = protocol.create_setting_message(repository.get_settings())
    port.write(msg)

    return asdict(repository.get_settings())