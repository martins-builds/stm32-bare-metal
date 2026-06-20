# I2C LCD

16x2 LCD display driven over I2C via a PCF8574 I/O expander backpack.
Displays text in 4-bit mode. No HAL — direct register manipulation only.

## How It Works

### Hardware Chain

The LCD (HD44780 controller) communicates in parallel (4-bit or 8-bit).
The PCF8574 backpack sits between the STM32 and LCD, converting I2C
into the parallel signals the LCD expects. The STM32 sends one byte
per nibble over I2C to the PCF8574 at address 0x27.
STM32 → I2C (PB8/PB9) → PCF8574 (0x27) → LCD HD44780 (4-bit mode)

### I2C Configuration

I2C1 on PB8 (SCL) and PB9 (SDA), alternate function 4, open-drain.

| Register | Value | Derivation |
|----------|-------|------------|
| CR2 | 45 | APB1 clock in MHz (45MHz) |
| CCR | 225 | 45MHz / (2 × 100kHz) = 225 |
| TRISE | 46 | (1000ns × 45MHz / 1000) + 1 = 46 |

Standard mode (100kHz). APB1 runs at 45MHz (SYSCLK 180MHz / APB1 prescaler /4).

### 4-bit Mode and Nibble Sending

The HD44780 in 4-bit mode receives each byte as two nibbles — high
nibble first, then low nibble. Each nibble is latched by pulsing the
Enable (EN) pin high then low. The RS pin selects command (0) or data (1).

PCF8574 bit mapping:
bit 7 = DB7    bit 3 = EN

bit 6 = DB6    bit 2 = RW

bit 5 = DB5    bit 1 = RS

bit 4 = DB4    bit 0 = Backlight

Each nibble write: set data bits + RS + backlight → pulse EN high → pulse EN low.

### LCD Initialization Sequence

HD44780 requires a specific reset sequence on power-up before 4-bit
mode can be used reliably:
Wait 50ms (power stabilize)

Send 0x30 → wait 5ms      (function set, 8-bit)

Send 0x30 → wait 150µs    (function set, 8-bit)

Send 0x30 → wait 150µs    (function set, 8-bit)

Send 0x20                  (switch to 4-bit mode)

Now in 4-bit mode:

0x28 → 2 lines, 5×8 font

0x0C → display on, cursor off

0x06 → entry mode, increment cursor

0x01 → clear display (wait 2ms)

Skipping the triple 0x30 sequence causes random init failures on cold boot.

### DWT Microsecond Delay

SysTick provides 1ms resolution. The LCD EN pulse requires 1µs timing,
so the DWT cycle counter is used instead. At 180MHz, one cycle = 5.5ns.
cycles = µs × 180   (180 cycles per microsecond at 180MHz)
DWT is a Cortex-M4 debug peripheral enabled via DEMCR bit 24 and
DWT_CTRL bit 0. No RCC gating needed.

## Peripherals Used
- **RCC** — PLL config, GPIOA/GPIOB (AHB1), I2C1 and USART2 (APB1)
- **FLASH** — 5 wait states, instruction cache, data cache, prefetch
- **GPIO** — PB8 AF4 (I2C1 SCL), PB9 AF4 (I2C1 SDA), PA2 AF7 (USART2 TX), PA3 AF7 (USART2 RX)
- **I2C1** — 100kHz standard mode, master transmit
- **USART2** — 9600 baud serial (available for debug)
- **SysTick** — 1ms interrupt-driven delay
- **DWT** — cycle counter for microsecond delays

## Wiring

| PCF8574 Pin | Connect To |
|-------------|------------|
| VCC | 5V |
| GND | GND |
| SDA | PB9 |
| SCL | PB8 |

> ⚠️ Pull-up resistors (4.7kΩ) required on SDA and SCL to 3.3V.
> Most PCF8574 LCD backpack modules include these onboard — check
> your module before adding external ones.

> ⚠️ The LCD backpack runs on 5V but PCF8574 SDA/SCL are 5V tolerant
> on most modules. Verify your specific module's logic level before
> connecting directly to STM32 3.3V GPIO.

## Flash
make flash

## Built With
- arm-none-eabi-gcc
- st-flash
- STM32F446RE reference manual (RM0390)
- HD44780 datasheet
- PCF8574 datasheet