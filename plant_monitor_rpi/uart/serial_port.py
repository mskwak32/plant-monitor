import serial

_PORT = "/dev/ttyACM0"
_BAUD = 115200

class SerialPort:
    def __init__(self, port: str = _PORT, baud: int = _BAUD):
        self._ser = serial.Serial(port, baud, timeout=1)
        
    def readline(self) -> str:
        return self._ser.readline().decode("utf-8", errors="ignore").strip()
    
    def write(self, data: str) -> None:
        self._ser.write(data.encode("utf-8"))
        
    def close(self) -> None:
        self._ser.close()