# 센서 API 라우터
from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from typing import Optional
from db import repository

router = APIRouter()

# 응답 모델 정의
class SensorResponse(BaseModel):
    id: int
    timestamp: str
    soil_moisture_pct: Optional[int]
    air_temperature: Optional[float]
    air_humidity: Optional[float]
    
# GET /sensors/latest
@router.get("/sensors/latest", response_model=SensorResponse)
async def get_latest():
    row = repository.get_latest_sensor()
    if row is None:
        raise HTTPException(status_code=404, detail="센서 데이터 없음")
    return dict(row)

# GET /sensors/history
@router.get("/sensors/history", response_model=list[SensorResponse])
async def get_history(limit: int = 100):
    rows = repository.get_sensor_history(limit)
    return [dict(row) for row in rows]