# UART

USART2 transmit and receive over serial at 9600 baud.
No HAL — direct register manipulation only.

## How It Works
USART2 is configured on PA2 (TX) and PA3 (RX) using GPIO alternate
function 7. Prints "Hello from STM32" every second using SysTick
for timing.

## Peripherals Used
- **RCC** — clock enable for GPIOA (AHB1) and USART2 (APB1)
- **GPIO** — PA2 and PA3 in alternate function mode (AF7)
- **USART2** — 9600 baud, transmit and receive
- **SysTick** — 1ms interrupt-driven delay

## Key Concepts
- GPIOA_MODER bits set to 10 for alternate function mode
- GPIOA_AFRL bits 11:8 = 0111 (AF7) for PA2, bits 15:12 = 0111 for PA3
- USART_BRR = F_CPU / baud = 16000000 / 9600 = 1666
- USART_CR1: UE(13), TE(3), RE(2) to enable USART, TX and RX
- Poll TXE (bit 7) in SR before writing to DR
- Poll RXNE (bit 5) in SR before reading from DR

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