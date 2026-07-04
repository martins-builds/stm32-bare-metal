# FreeRTOS Capstone

Multitasking system combining ADC, RFID, I2C LCD, and USART under FreeRTOS.
Three concurrent tasks communicate via queues and a mutex. No HAL —
direct register manipulation only, FreeRTOS handles scheduling.

## How It Works

### System Overview

This combines every peripheral built in earlier projects into one
running system under FreeRTOS:
ADC Task  → reads potentiometer, sends mV to xAdcQueue every 300ms
RFID Task → polls RC522 for card UID, sends to xRfidQueue every 500ms
Display Task → blocks on xAdcQueue, checks xRfidQueue, prints both
to USART2 and LCD, protected by xUartMutex

### Task Architecture

| Task | Priority | Stack | Behavior |
|------|----------|-------|----------|
| ADC | 1 | 256 words | Polls ADC, sends result to queue every 300ms |
| RFID | 1 | 256 words | Polls RC522, sends UID to queue every 500ms |
| Display | 1 | 256 words | Blocks on ADC queue, checks RFID queue, prints |

All tasks run at equal priority — none are time-critical relative
to the others, so the scheduler round-robins between them on each
`vTaskDelay` yield.

### Queues

```c
xAdcQueue  = xQueueCreate(10, sizeof(uint16_t));   // ADC mV values
xRfidQueue = xQueueCreate(5, sizeof(uint8_t[4]));  // 4-byte UIDs
```

`display_task` blocks indefinitely on `xAdcQueue` (`portMAX_DELAY`)
since ADC data arrives continuously. It checks `xRfidQueue` with a
0 timeout — non-blocking — since a card may or may not be present
on any given cycle.

### Mutex

```c
xUartMutex = xSemaphoreCreateMutex();
```

Protects USART2 and LCD output inside `display_task`. Only one task
currently prints, but the mutex pattern is in place for when
additional tasks need to share the output peripherals.

### DMA — Present But Not Used

`dma_adc_init()` and the DMA2 Stream0 register definitions are
present in the code from the earlier dma-adc project, but are not
called in `main()`. The ADC task in this build uses polling
(`adc_read()`) instead of DMA. The DMA path is left in the source
as reference but is dead code in this version.

## Peripherals Used
- **RCC** — PLL config, GPIOA/GPIOB (AHB1), I2C1, SPI1, ADC1, USART2
- **FLASH** — 5 wait states, instruction cache, data cache, prefetch
- **GPIO** — PA0 ADC, PA2/PA3 USART2, PA4-PA7 SPI1, PB8/PB9 I2C1
- **ADC1** — 12-bit, single conversion polling, channel 0
- **I2C1** — 100kHz standard mode, PCF8574 LCD backpack
- **SPI1** — Mode 0, master, 5.625MHz, RC522 RFID reader
- **USART2** — 9600 baud serial output
- **DWT** — cycle counter for microsecond delays, used for busy-wait
- **FreeRTOS** — 3 tasks, 2 queues, 1 mutex, preemptive scheduler

## Wiring

| Peripheral | Pin | Connect To |
|------------|-----|------------|
| USART2 TX | PA2 | Onboard ST-Link |
| USART2 RX | PA3 | Onboard ST-Link |
| I2C1 SCL | PB8 | PCF8574 SCL |
| I2C1 SDA | PB9 | PCF8574 SDA |
| ADC1 CH0 | PA0 | Potentiometer wiper |
| SPI1 SCK | PA5 | RC522 SCK |
| SPI1 MISO | PA6 | RC522 MISO |
| SPI1 MOSI | PA7 | RC522 MOSI |
| SPI1 CS | PA4 | RC522 SDA(CS) |

**RC522 RST** — wired directly to 3.3V (held permanently high,
not under software control). **RC522 IRQ** — not wired, unused
in this build since card detection is done by polling.

> ⚠️ ADC and RC522 input max is 3.3V.  and PCF8574 LCD run on 5V.

## View Output
screen /dev/tty.usbmodem14103 9600
To exit screen: Ctrl+A then K

## Flash
make flash

## Built With
- arm-none-eabi-gcc
- st-flash
- FreeRTOS kernel
- STM32F446RE reference manual (RM0390)
- MFRC522 datasheet