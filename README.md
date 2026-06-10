# STM32 Bare-Metal

Bare-metal embedded systems projects for the STM32F446RE (Nucleo-64).
No HAL, no CubeIDE, no abstractions. Direct register manipulation only.

## Projects

| Project | Description | Peripherals |
|---------|-------------|-------------|
| [blink](./blink/) | LED blink with busy-wait delay | RCC, GPIO |
| [gpio-input](./gpio-input/) | Button interrupt toggles LED | RCC, GPIO, SYSCFG, EXTI, NVIC |
| [systick](./systick) | Precise 1ms blink using SysTick timer | RCC, GPIO, SysTick |

## Toolchain
- arm-none-eabi-gcc
- st-flash
- macOS + Homebrew

## Key Differences from AVR
- Clock gating required via RCC before any peripheral works
- Each GPIO pin uses 2 bits in MODER (not 1 like AVR)
- Alternate function register (AFRL/AFRH) needed for UART, SPI, I2C
- Linker script and startup file written manually
- 32-bit registers, Cortex-M4 architecture with NVIC interrupt controller
- SYSCFG required to route GPIO pins to EXTI lines
- SysTick is a Cortex-M4 core peripheral — no RCC gating needed

## Flash Any Project
cd <project-folder>
make flash