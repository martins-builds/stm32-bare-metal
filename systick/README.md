# SysTick Blink

Precise millisecond LED blink using the Cortex-M4 SysTick timer.
Replaces the busy-wait delay from the first blink project with an
accurate interrupt-driven millisecond counter.

## How It Works
SysTick is configured to fire every 1ms at 16MHz (reload = 15999).
The SysTick_Handler ISR increments a volatile tick counter.
delay_ms() compares the current tick against a start value to
create a non-blocking style delay.

## Peripherals Used
- **SysTick** — Cortex-M4 system timer, interrupt every 1ms
- **RCC** — clock enable for GPIOA
- **GPIO** — PA5 output (onboard LED LD2)

## Key Concepts
- SYST_RVR = (F_CPU / 1000) - 1 = 15999 for 1ms at 16MHz
- SYST_CVR cleared to 0 to start counting immediately
- SYST_CSR bits: ENABLE(0), TICKINT(1), CLKSOURCE(2) all set to 1
- tick declared volatile — modified in ISR, read in main
- SysTick vector is position 15 in the vector table

## Wiring
Onboard only.
- LED — PA5 (LD2, green onboard LED)

## Built With
- arm-none-eabi-gcc
- st-flash
- Cortex-M4 Generic User Guide — SysTick register descriptions

## Flash
make flash