# Plant Monitor Portfolio Project

## Project Structure
- `plant_monitor_stm32/` — STM32 microcontroller firmware (C/C++, HAL library)
- `plant_monitor_rpi/` — Raspberry Pi 5 application (Linux environment)

## Dev Environment
- STM32: STM32CubeIDE 2.1.1 on macOS, STM32CubeMX 6.17.0
  - Board: STM32 Nucleo-F103RB (STM32F103RBT6, 128KB Flash, 20KB SRAM)
- RPi5: VS Code via SSH

## Hardware
See `portfolio_system_architecture.md` (parts list) and `STM32 Nucleo-F103RB User manual.pdf` (pin mappings).
- Soil sensor: SZH-EK106 (ADC)
- Temp/humidity: RHT-01 (GPIO bit-bang) — see `RHT01.pdf`
- Relay: 5V 1-ch, LOW-active (JQC-3FF-S-Z)
- Pump: DC 12V
- OLED: SSD1306 I2C, connected to STM32
- RPi5 8GB: web dashboard server only

## Git
- `stm32` branch — STM32 work
- `rpi` branch — RPi5 work
- Commit messages: focus on change, no week numbers.

## Weekly Summary
- At the end of each week, create `{N}주차_맥락.md` summarizing the week's work.
- Use the previous week's file as a format reference.

## Rules
- STM32 and RPi code must stay in their respective folders.
- STM32: C/C++ with HAL. RPi: Python or C++.
- User writes all code. Claude provides explanations and guidance only.
- User is learning embedded from scratch. For every code block, explain: what it does, why it's needed, what breaks if omitted.
