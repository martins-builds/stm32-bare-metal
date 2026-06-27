# SPI RC522

MFRC522 RFID reader over SPI1. Reads 4-byte card UID and prints
it over USART2 in hex. No HAL — direct register manipulation only.

## How It Works

### SPI Configuration

SPI1 on PA5 (SCK), PA6 (MISO), PA7 (MOSI), PA4 (CS). All SPI pins
use alternate function 5. CS is manual GPIO output on PA4.

| Register | Value | Purpose |
|----------|-------|---------|
| CPHA bit 0 | 0 | Clock phase — sample on first edge |
| CPOL bit 1 | 0 | Clock polarity — idle low (Mode 0) |
| MSTR bit 2 | 1 | Master mode |
| BR 5:3 | 011 | Prescaler /16 → 90MHz/16 = 5.625MHz |
| SSM bit 9 | 1 | Software slave management |
| SSI bit 8 | 1 | Internal slave select high |
| SPE bit 6 | 1 | SPI enable |

RC522 max SPI clock is 10MHz. 5.625MHz is within spec.

### RC522 Register Protocol

RC522 SPI frame format — address byte first, then data byte:
Write: CS low → (reg << 1) & 0x7E → value → CS high

Read:  CS low → ((reg << 1) & 0x7E) | 0x80 → 0x00 (dummy) → CS high
Bit 7 of address byte = 0 for write, 1 for read. Bits 6:1 = register
address. Bit 0 always 0.

### Card Detection Flow
rc522_request()

→ sets BitFramingReg (0x0D) to 7-bit frame

→ sends REQA command (0x26) via transceive

→ card responds with ATQA if present
rc522_anticollision()

→ sends SELECT command (0x93, 0x20)

→ card responds with 4-byte UID + checksum byte

→ copies first 4 bytes as UID
rc522_transceive()

→ clears ComIrqReg (0x04)

→ flushes FIFO via FIFOLevelReg (0x0A)

→ writes data to FIFODataReg (0x09)

→ sets Transceive command (0x0C) in CommandReg (0x01)

→ polls ComIrqReg for RxIRq (bit 5) or TimerIRq (bit 0)

→ reads response bytes from FIFO

### UID Output

UID printed as hex bytes over USART2:
AB OC 00 08

## Peripherals Used
- **RCC** — PLL config, GPIOA (AHB1), SPI1 and USART2 (APB2/APB1)
- **FLASH** — 5 wait states, instruction cache, data cache, prefetch
- **GPIO** — PA4 CS (output), PA5 AF5 SCK, PA6 AF5 MISO, PA7 AF5 MOSI, PA2 AF7 TX, PA3 AF7 RX
- **SPI1** — Mode 0, master, 5.625MHz, 8-bit
- **USART2** — 9600 baud serial output
- **SysTick** — 1ms interrupt-driven delay
- **DWT** — cycle counter for microsecond delays

## Wiring

| RC522 Pin | Connect To |
|-----------|------------|
| SDA (CS) | PA4 |
| SCK | PA5 |
| MISO | PA6 |
| MOSI | PA7 |
| GND | GND |
| VCC | 3.3V |
| RST | 3.3V |

## View Output
screen /dev/tty.usbmodem14103 9600
To exit screen: Ctrl+A then K

## Flash
make flash

## Built With
- arm-none-eabi-gcc
- st-flash
- STM32F446RE reference manual (RM0390)
- MFRC522 datasheet