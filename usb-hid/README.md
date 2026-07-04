# USB CDC (TinyUSB)

STM32F446RE appearing as a virtual serial port over USB, using TinyUSB
for enumeration and CDC class handling. Bare-metal clock, GPIO, NVIC,
and SysTick configuration — TinyUSB handles the USB stack.
Debug output also available over USART2.

## How It Works

### Why the PLL changed

Previous projects ran SYSCLK at 180MHz via PLLN=360. USB requires
a strict 48MHz clock on the OTG_FS peripheral. The 48MHz clock comes
from the main PLL via the PLLQ divider:
USB clock = (HSE / PLLM) × PLLN / PLLQ
= (8MHz / 8)   × 336  / 7
= 1MHz × 336 / 7
= 48MHz ✓

SYSCLK changes as a result:
SYSCLK = (HSE / PLLM) × PLLN / PLLP
= 1MHz × 336 / 2
= 168MHz

| Parameter | Previous projects | This project |
|-----------|------------------|--------------|
| PLLN | 360 | 336 |
| PLLQ | — | 7 |
| SYSCLK | 180MHz | 168MHz |
| USB clock | — | 48MHz |
| APB1 | 45MHz | 42MHz |
| APB2 | 90MHz | 84MHz |

### USB GPIO

PA11 (D−) and PA12 (D+) configured as alternate function 10 (OTG_FS).
Both set to alternate function mode (MODER = 10) via GPIOA_AFRH.

### OTG_FS Interrupt

OTG_FS is IRQ67. It sits in NVIC_ISER2, bit 3 (67 − 64 = 3).
`OTG_FS_IRQHandler` routes directly to `tud_int_handler(0)`.

### TinyUSB

TinyUSB handles USB enumeration, CDC class descriptors, FIFO
management, and data transfer. The application layer only needs:

```c
tusb_init();        // initialize the stack
tud_task();         // call in main loop — processes USB events
tud_cdc_connected() // check if host has opened the port
tud_cdc_write_str() // send string to host
tud_cdc_write_flush()
```

### SysTick

SysTick configured at 168MHz for 1ms ticks. `tusb_time_millis_api()`
returns `tusb_tick` — required by TinyUSB for internal timing.

### USART2

Kept for debug output at 9600 baud. APB1 = 42MHz at 168MHz SYSCLK.
USART_BRR = 42000000 / 9600 = 4375

## Peripherals Used
- **RCC** — PLL at 168MHz, PLLQ=7 for 48MHz USB clock, GPIOA (AHB1), OTG_FS (AHB2), USART2 (APB1)
- **FLASH** — 5 wait states, instruction cache, data cache, prefetch
- **GPIO** — PA11 AF10 (D−), PA12 AF10 (D+), PA2 AF7 (USART2 TX), PA3 AF7 (USART2 RX)
- **OTG_FS** — full-speed USB device, IRQ67
- **SysTick** — 1ms tick for TinyUSB timing
- **TinyUSB** — USB stack, CDC class, enumeration, FIFO

## Wiring

| Pin | Connect To |
|-----|------------|
| PA11 | USB D− |
| PA12 | USB D+ |
| PA2 | Onboard ST-Link (USART2 TX) |
| PA3 | Onboard ST-Link (USART2 RX) |

Connect a USB cable from the STM32 USB connector (not ST-Link) to
your computer. The board will enumerate as a virtual serial port.

## View USB Output
screen /dev/tty.usbmodem<id> 115200

## View USART Debug Output
screen /dev/tty.usbmodem14103 9600
To exit screen: Ctrl+A then K

## Flash
make flash

## Built With
- arm-none-eabi-gcc
- st-flash
- TinyUSB
- STM32F446RE reference manual (RM0390)