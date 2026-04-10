# Plant Monitor Portfolio Project

## Project Structure
- `plant_monitor_stm32/` — STM32 microcontroller firmware (C/C++, HAL library)
- `plant_monitor_rpi/` — Raspberry Pi 5 application (Linux environment)

## Dev Environment
- STM32: STM32CubeIDE for Visual Studio Code, STM32CubeMX 6.17.0
  - Board: STM32 Nucleo-F103RB (STM32F103RBT6, 128KB Flash, 20KB SRAM)
- RPi5: VS Code via SSH

## Hardware
- Soil sensor: SZH-EK106 (ADC)
- Temp/humidity: RHT-01 (GPIO bit-bang) — see `RHT01.pdf`
- Relay: 5V 1-ch, HIGH-active (JQC-3FF-S-Z)
- Pump: DC 12V
- OLED: SSD1306 I2C, connected to STM32
- RPi5 8GB: web dashboard server only

## Reference Docs
Consult these proactively when relevant — do not wait for the user to ask:
- `system_architecture.md` — overall system architecture, parts list, wiring
- `stm32f103rb.pdf` — STM32F103RB datasheet (registers, peripherals, electrical specs)
- `STM32 Nucleo-F103RB User manual.pdf` — Nucleo board pin mapping, jumpers, schematics

## Git
- `stm32` branch — STM32 work
- `rpi` branch — RPi5 work
- Commit message format: `{N}주차: <변경 내용>` (e.g., `5주차: SSD1306 I2C 드라이버 구현`)
  - Do NOT use conventional commit prefixes (feat:, fix:, chore:, refactor:, etc.)

## Rules
- STM32 and RPi code must stay in their respective folders.
- STM32: C/C++ with HAL. RPi: Python or C++.
- Codex writes code only when the user explicitly asks. Otherwise, provide explanations and guidance only.
- User is learning embedded from scratch. For every code block, explain: what it does, why it's needed, what breaks if omitted.
