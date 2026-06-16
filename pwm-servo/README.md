# UART + PLL

USART2 transmit and receive over serial at 9600 baud, running at 180MHz
system clock via PLL. No HAL — direct register manipulation only.

## How It Works

### Clock Chain: HSE → PLL → SYSCLK

The STM32F446RE boots from the internal HSI oscillator at 16MHz.
This project switches to the external HSE oscillator (8MHz on the Nucleo)
and feeds it through the PLL to reach 180MHz SYSCLK.

The PLL has three dividers that control this chain:
HSE (8MHz) → ÷M → PLL input → ×N → VCO → ÷P → SYSCLK

8MHz → ÷8 → 1MHz  → ×360 → 360MHz → ÷2 → 180MHz
| Divider | Value | Purpose |
|---------|-------|---------|
| M | 8 | Scales HSE down to 1MHz for PLL input |
| N | 360 | Multiplies up to 360MHz VCO frequency |
| P | 2 | Divides VCO down to 180MHz SYSCLK |

### Bus Prescalers

AHB, APB1 and APB2 each have separate prescalers set in RCC_CFGR:

| Bus | Prescaler | Speed | Constraint |
|-----|-----------|-------|------------|
| AHB | /1 | 180MHz | Max 180MHz |
| APB1 | /4 | 45MHz | Max 45MHz |
| APB2 | /2 | 90MHz | Max 90MHz |

USART2 sits on APB1, so its clock is 45MHz. BRR is calculated as:
`USART_BRR = 45000000 / 9600 = 4687`

### Flash Latency

At 180MHz the CPU fetches instructions faster than flash can serve them.
Flash latency must be set to 5 wait states before switching to PLL —
if done after, the CPU outruns flash and executes garbage. Instruction
cache, data cache and prefetch are also enabled to recover lost cycles
from the wait states.

### USART2 Configuration

Configured on PA2 (TX) and PA3 (RX) using GPIO alternate function 7.
Prints "Hello from STM32" every second using SysTick for timing.

## Peripherals Used
- **RCC** — PLL configuration, clock enable for GPIOA and USART2
- **FLASH** — wait state and cache configuration
- **GPIO** — PA2 and PA3 in alternate function mode (AF7)
- **USART2** — 9600 baud, transmit and receive on APB1 at 45MHz
- **SysTick** — 1ms interrupt-driven delay, reload = 180000000/1000 - 1

## Key Concepts
- Flash latency must be set BEFORE switching SYSCLK to PLL
- SWS bits (3:2) in RCC_CFGR confirm the clock switch completed
- VCO frequency must stay between 100-432MHz (360MHz here)
- PLL input after M divider should be 1-2MHz (1MHz here)
- APB1 max 45MHz, APB2 max 90MHz per F446RE datasheet

## Wiring
Onboard only — Nucleo ST-Link bridges USART2 to USB automatically.
- TX — PA2
- RX — PA3

## View Output
screen /dev/tty.usbmodem14103 9600
To exit screen: Ctrl+A then K

## Flash
make flash

## Built With
- arm-none-eabi-gcc
- st-flash
- STM32F446RE reference manual (RM0390)