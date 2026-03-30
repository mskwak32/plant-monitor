# Plant Monitor Portfolio Project

## Project Structure
- `plant_monitor_stm32/` — STM32 microcontroller firmware (C/C++, HAL library)
- `plant_monitor_rpi/` — Raspberry Pi 5 application (Linux environment)

## Dev Environment
- STM32: STM32CubeIDE 2.1.1 on macOS, STM32CubeMX 6.17.0
  - Board: STM32 Nucleo-F103RB (STM32F103RBT6, 128KB Flash, 20KB SRAM)
- RPi5: VS Code via SSH

## Hardware Components
See `portfolio_system_architecture.md` for the full parts list and `STM32 Nucleo-F103RB User manual.pdf` for pin mappings. Key components:
- MCU: STM32 Nucleo-F103RB
- Soil moisture sensor: SZH-EK106 (analog, ADC)
- Temp/humidity sensor: RHT-01 (digital, GPIO bit-bang) — see `RHT01.pdf` for protocol details
- Relay module: 5V 1-ch (LOW-active, controls DC pump) — JQC-3FF-S-Z based
- Water pump: DC 12V (powered by separate 12V adapter)
- OLED display: 0.96" I2C SSD1306 — connected to **STM32** (shows soil moisture, threshold, temp/humidity, pump status)
- RPi5 8GB — web dashboard server only

## Git Workflow
- `stm32` branch — STM32 work
- `rpi` branch — RPi5 work
- Sync via GitHub

## Rules
- Keep STM32 and RPi code strictly separated in their respective folders.
- STM32: use C/C++ with STM32 HAL. RPi: use Python or C++ for Linux.
