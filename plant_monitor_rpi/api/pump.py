# 펌프 이력 API 라우터
from fastapi import APIRouter
from pydantic import BaseModel
from db import repository

router = APIRouter()

class PumpLogResponse(BaseModel):
    id: int
    timestamp: str
    action: str
    
@router.get("/pump/logs", response_model=list[PumpLogResponse])
async def get_pump_logs(limit: int = 100):
    rows = repository.get_pump_logs(limit)
    return [dict(row) for row in rows]