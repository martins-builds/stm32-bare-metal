# GPIO Input — Button Interrupt

Button-triggered LED toggle using external interrupt on the STM32F446RE.
No HAL, no abstractions — direct register manipulation only.

## How It Works
The onboard user button (B1) on PC13 triggers an EXTI13 falling edge
interrupt. The ISR clears the pending flag and toggles the LED on PA5.
No polling — fully interrupt driven.

## Peripherals Used
- **RCC** — clock enable for GPIOA, GPIOC, SYSCFG
- **GPIO** — PA5 output (LED), PC13 input with pull-up (button)
- **SYSCFG** — connects PC13 to EXTI line 13 via EXTICR4
- **EXTI** — falling edge trigger on line 13, interrupt mask
- **NVIC** — enables EXTI15_10 IRQ (IRQ40) via ISER1

## Wiring
Onboard only — no external components needed.
- LED — PA5 (LD2, green onboard LED)
- Button — PC13 (B1, blue onboard button)

## Key Concepts
- SYSCFG_EXTICR4 bits 7:4 = 0010 to select port C for EXTI13
- EXTI_FTSR bit 13 — falling edge (pin goes LOW when button pressed)
- EXTI_PR bit 13 must be cleared in ISR by writing 1 to it
- Vector table position 56 for EXTI15_10 (IRQ40 + 16 system exceptions)
- NVIC_ISER1 bit 8 enables IRQ40

## Built With
- arm-none-eabi-gcc
- st-flash
- STM32F446RE reference manual (RM0390) — Table 38 for vector positions

## Flash
make flash