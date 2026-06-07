# STM32 Bare-Metal

Bare-metal embedded systems projects for the STM32F446RE (Nucleo-64).
No HAL, no CubeIDE, no abstractions. Direct register manipulation only.

## Projects

| Project | Description | Peripherals |
|---------|-------------|-------------|
| [blink](./blink/) | LED blink via direct register access | RCC, GPIO |

## Toolchain
- arm-none-eabi-gcc
- st-flash
- macOS + Homebrew

## Key differences from AVR
- Clock gating required (RCC) before any peripheral
- Each GPIO pin uses 2 bits in MODER (not 1)
- Linker script and startup file written manually
- 32-bit registers, Cortex-M4 architecture

## Flash any project
cd <project-folder>
make flash