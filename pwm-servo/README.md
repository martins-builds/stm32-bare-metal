# PWM Servo

TIM2 PWM output on PA0 controlling a servo motor at 50Hz.
No HAL — direct register manipulation only.

## How It Works

### Clock Chain: HSE → PLL → SYSCLK → TIM2

The system runs at 180MHz via PLL from the 8MHz HSE oscillator.
TIM2 sits on APB1 which runs at 45MHz, but APB1 timers receive
a ×2 multiplier when the APB prescaler is not /1, giving TIM2
a 90MHz clock.
HSE (8MHz) → ÷8 → ×360 → ÷2 → SYSCLK 180MHz

→ AHB /1  → 180MHz

→ APB1 /4 → 45MHz → TIM2 ×2 → 90MHz

### PWM Frequency: 50Hz

Servos require a 50Hz PWM signal — one pulse every 20ms.
TIM2 is configured with:

| Register | Value | Purpose |
|----------|-------|---------|
| PSC | 89 | Prescaler: 90MHz / 90 = 1MHz timer tick |
| ARR | 19999 | Period: 1MHz / 20000 = 50Hz |
| CCR1 | variable | Pulse width → servo angle |

Timer tick = 90MHz / (PSC+1) = 90MHz / 90 = 1MHz = 1µs per tick

Period     = (ARR+1) ticks   = 20000µs = 20ms = 50Hz

### Angle to Pulse Width

Standard servo protocol:
- 1ms pulse = 0°
- 1.5ms pulse = 90°
- 2ms pulse = 180°

CCR1 formula:
CCR1 = 1000 + (angle × 1000 / 180)

| Angle | CCR1 | Pulse Width |
|-------|------|-------------|
| 0°    | 1000 | 1.0ms |
| 90°   | 1500 | 1.5ms |
| 180°  | 2000 | 2.0ms |

### TIM2 PWM Mode 1

PWM Mode 1 (OC1M = 110): output is high while TIM2_CNT < CCR1,
low otherwise. Configured on channel 1 (PA0, AF1).

- CCMR1 bits 6:4 = 110 (PWM Mode 1)
- CCER bit 0 = 1 (CC1E, channel 1 output enable)
- CR1 bit 7 = 1 (ARPE, auto-reload preload enable)
- EGR bit 0 = 1 (UG, update event to latch PSC and ARR)

## Peripherals Used
- **RCC** — PLL config, GPIOA (AHB1), TIM2 and USART2 (APB1)
- **FLASH** — 5 wait states, instruction cache, data cache, prefetch
- **GPIO** — PA0 AF1 (TIM2_CH1), PA2 AF7 (USART2 TX), PA3 AF7 (USART2 RX)
- **TIM2** — 50Hz PWM output on channel 1
- **USART2** — 9600 baud serial debug output
- **SysTick** — 1ms interrupt-driven delay

## Wiring

| Servo Wire | Connect To |
|------------|------------|
| Signal (orange/yellow) | PA0 |
| Power (red) | 5V |
| Ground (brown/black) | GND |

> ⚠️ Power the servo from the Nucleo 5V pin or an external 5V supply.
> Do not power it from 3.3V — servos require 5V and draw more current
> than the STM32 can safely provide directly.

## View Output
screen /dev/tty.usbmodem14103 9600

To exit screen: Ctrl+A then K

## Flash
make flash

## Built With
- arm-none-eabi-gcc
- st-flash
- STM32F446RE reference manual (RM0390)