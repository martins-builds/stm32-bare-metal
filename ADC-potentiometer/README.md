# ADC Potentiometer

12-bit ADC reading from a potentiometer on PA0, converted to millivolts
and printed over USART2 at 200ms intervals. No HAL — direct register
manipulation only.

## How It Works

### ADC Clock Chain

ADC1 is on APB2 which runs at 90MHz (SYSCLK 180MHz / APB2 prescaler /2).
The ADC peripheral has a maximum input clock of 36MHz, so an additional
prescaler is applied inside the ADC common registers:
SYSCLK 180MHz → APB2 /2 → 90MHz → ADC prescaler /4 → 22.5MHz
| Register | Value | Purpose |
|----------|-------|---------|
| RCC_CFGR APB2 | /2 | APB2 = 90MHz |
| ADC_CCR ADCPRE | /4 (10) | ADC clock = 22.5MHz, within 36MHz limit |

### ADC Configuration

ADC1 channel 0 on PA0 in analog mode (MODER bits 1:0 = 11).

| Register | Value | Purpose |
|----------|-------|---------|
| CR1 RES 25:24 | 00 | 12-bit resolution (0–4095) |
| SQR3 | 0 | Channel 0 as first conversion in sequence |
| SMPR2 2:0 | 111 | 480 cycle sample time — maximum, best accuracy |
| CR2 ADON | 1 | ADC enabled |
| CR2 SWSTART | bit 30 | Software trigger to start conversion |

### Voltage Conversion Formula
voltage_mV = (raw × 3300) / 4095
- raw → 12-bit ADC result (0–4095)
- 3300 → VREF = 3.3V = 3300mV
- 4095 → maximum 12-bit value (2¹² − 1)
- `3300UL` used to prevent uint16 overflow during multiplication

| Raw Value | Voltage |
|-----------|---------|
| 0 | 0 mV |
| 2047 | ~1650 mV |
| 4095 | 3300 mV |

### Sample Time

480 cycles at 22.5MHz = ~21.3µs per conversion. Intentionally slow
for a potentiometer — longer sample time lets the ADC input capacitor
fully charge, improving accuracy especially at signal extremes.

## Peripherals Used
- **RCC** — PLL config, GPIOA (AHB1), ADC1 (APB2), USART2 (APB1)
- **FLASH** — 5 wait states, instruction cache, data cache, prefetch
- **GPIO** — PA0 analog input, PA2 AF7 (USART2 TX), PA3 AF7 (USART2 RX)
- **ADC1** — 12-bit, single conversion, software trigger, channel 0
- **ADC_CCR** — prescaler /4, ADC clock 22.5MHz
- **USART2** — 9600 baud serial output
- **SysTick** — 1ms interrupt-driven delay
- **DWT** — cycle counter (available, not used in main loop)

## Wiring

| Potentiometer Pin | Connect To |
|-------------------|------------|
| Left leg | GND |
| Wiper (middle) | PA0 |
| Right leg | 3.3V |

> ⚠️ Do not connect the potentiometer to 5V. ADC input pins on the
> STM32F446RE are 3.3V max. Exceeding this damages the ADC.

## View Output
screen /dev/tty.usbmodem14103 9600

To exit screen: Ctrl+A then K

## Flash
make flash

## Built With
- arm-none-eabi-gcc
- st-flash
- STM32F446RE reference manual (RM0390)