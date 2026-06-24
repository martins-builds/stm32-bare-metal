# DMA ADC

ADC1 continuous conversion on PA0 transferred to memory via DMA2,
printed over USART2 as millivolts at 200ms intervals. CPU never
touches ADC_DR — DMA handles every transfer automatically.
No HAL — direct register manipulation only.

## How It Works

### Why DMA Here

Without DMA, the CPU polls EOC (end of conversion) and reads ADC_DR
manually for every sample. In continuous mode at high sample rates,
this wastes CPU cycles waiting on hardware. With DMA, ADC raises a
request after each conversion, DMA transfers ADC_DR → memory, and
the CPU is free to do other work — only reading the result when needed.

### Data Flow
Potentiometer → PA0 → ADC1 CH0 (continuous)

↓ DMA request on each EOC

DMA2 Stream0 CH0

↓ transfers ADC1_DR → adc_dma_value

uint16_t adc_dma_value (RAM)

↓ CPU reads every 200ms

USART2 TX → serial output

### Clock Chain
SYSCLK 180MHz → APB2 /2 → 90MHz → ADC prescaler /4 → 22.5MHz
ADC maximum clock is 36MHz. Prescaler /4 keeps it at 22.5MHz safely.

### ADC Configuration

| Register | Value | Purpose |
|----------|-------|---------|
| CR1 RES 25:24 | 00 | 12-bit resolution |
| SQR3 | 0 | Channel 0 (PA0) |
| SMPR2 2:0 | 111 | 480 cycle sample time |
| CR2 ADON | bit 0 | ADC power on |
| CR2 CONT | bit 1 | Continuous conversion mode |
| CR2 DMA | bit 8 | DMA mode enable |
| CR2 DDS | bit 9 | DMA disable selection — keep issuing requests |
| CR2 SWSTART | bit 30 | Kick first conversion |
| ADC_CCR ADCPRE | 10 | Prescaler /4, ADC clock 22.5MHz |

`DDS` (bit 9) is critical — without it ADC stops issuing DMA requests
after the first transfer even in continuous mode.

### DMA2 Stream0 Configuration

ADC1 is mapped to DMA2 Stream0 Channel0 per Table 28 of RM0390.

| Register | Value | Purpose |
|----------|-------|---------|
| CHSEL 27:25 | 000 | Channel 0 → ADC1 |
| PSIZE 11:10 | 10 | Peripheral size 16-bit (ADC_DR is 16-bit) |
| MSIZE 13:12 | 10 | Memory size 16-bit (uint16_t) |
| MINC bit 10 | 1 | Memory increment — not needed for 1 item but kept |
| CIRC bit 5 | 1 | Circular mode — loops forever, NDTR auto-reloads |
| DIR 7:6 | 00 | Peripheral to memory (default) |
| NDTR | 1 | One transfer per DMA request |
| PAR | ADC1_DR | Source: ADC data register |
| M0AR | &adc_dma_value | Destination: RAM variable |

**Stream must be disabled before configuration.** The code waits for
EN bit to clear before writing any other register — writing to a
running stream is undefined behavior per RM0390.

### Circular Mode

With `NDTR=1` and `CIRC=1`, after each transfer DMA reloads NDTR to 1
and immediately arms for the next ADC request. The result is
`adc_dma_value` is always the latest ADC reading with zero CPU involvement.

### Voltage Formula
voltage_mV = (adc_dma_value × 3300) / 4095
`3300UL` prevents uint16 overflow during multiplication.

## Peripherals Used
- **RCC** — PLL config, GPIOA (AHB1), DMA2 (AHB1), ADC1 (APB2), USART2 (APB1)
- **FLASH** — 5 wait states, instruction cache, data cache, prefetch
- **GPIO** — PA0 analog input, PA2 AF7 (USART2 TX), PA3 AF7 (USART2 RX)
- **ADC1** — 12-bit, continuous mode, DMA requests enabled, channel 0
- **ADC_CCR** — prescaler /4, ADC clock 22.5MHz
- **DMA2 Stream0** — channel 0, peripheral→memory, circular, 16-bit
- **USART2** — 9600 baud serial output
- **SysTick** — 1ms interrupt-driven delay
- **DWT** — cycle counter for microsecond delays

## Wiring

| Potentiometer Pin | Connect To |
|-------------------|------------|
| Left leg | GND |
| Wiper (middle) | PA0 |
| Right leg | 3.3V |

> ⚠️ ADC input max is 3.3V. Never connect to 5V.

## View Output
screen /dev/tty.usbmodem14103 9600
To exit screen: Ctrl+A then K

## Flash
make flash

## Built With
- arm-none-eabi-gcc
- st-flash
- STM32F446RE reference manual (RM0390)