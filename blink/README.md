# STM32 Bare-Metal Blink

Bare-metal LED blink on STM32F446RE. No HAL, no CubeIDE.
Direct register manipulation using the reference manual.

## How it works
- RCC_AHB1ENR enables GPIOA clock (required before any GPIO access)
- GPIOA_MODER sets PA5 as output (bits 11:10 = 01)
- GPIOA_ODR toggles PA5 HIGH and LOW in a loop
- Delay is a busy-wait loop (SysTick comes later)

## Concepts demonstrated
- Clock gating via RCC (unique to STM32, not needed on AVR)
- GPIO MODER 2-bit mode configuration
- Bare-metal register access via memory-mapped addresses
- Linker script (flash/RAM memory regions)
- Startup file (vector table, data/bss init, Reset_Handler)

## Setup
- STM32F446RE Nucleo-64
- Built-in green LED on PA5
- arm-none-eabi-gcc + st-flash on macOS

## Flash
make flash