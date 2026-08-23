# USB HID Keyboard

STM32F446RE enumerating as a USB HID keyboard. Pressing the onboard
button on PC13 sends the letter 'H' to the host. No HAL — bare-metal
clock, GPIO, NVIC, and SysTick. TinyUSB handles HID class descriptors
and report handling.

## How It Works

### USB HID vs USB CDC

CDC makes the device appear as a serial port — data flows as a stream.
HID makes the device appear as an input device (keyboard, mouse,
gamepad). The host reads structured reports instead of raw bytes.
Each keypress is a report containing modifier keys and up to 6
simultaneous keycodes.

### Button to Keypress Flow
PC13 button pressed (active low)
→ tud_hid_keyboard_report(0, 0, keys)  // send 'H'
→ wait for button release
→ tud_hid_keyboard_report(0, 0, no_keys)  // send release
The release report is required — without it the host treats the key
as held indefinitely.

### HID Report Format

```c
uint8_t keys[6] = {HID_KEY_H, 0, 0, 0, 0, 0};
tud_hid_keyboard_report(0, 0, keys);
//                       ^  ^
//                       |  modifier byte (shift, ctrl, alt)
//                       report ID
```

Up to 6 simultaneous keys per report. Modifier byte handles Shift,
Ctrl, Alt etc. Both set to 0 here — plain 'h' with no modifiers.

### HID Callbacks

TinyUSB requires two callbacks even if unused:

```c
tud_hid_set_report_cb()  // host→device reports (LEDs, etc) — stubbed
tud_hid_get_report_cb()  // host requests a report — stubbed, returns 0
```

### Clock and USB — Same as USB CDC

PLLN=336, PLLQ=7 → 48MHz USB clock. SYSCLK = 168MHz.
PA11 (D−) and PA12 (D+) both AF10. OTG_FS IRQ67 via NVIC_ISER2 bit 3.

### PC13 Button

PC13 is the onboard blue user button on the Nucleo-64. Configured
with internal pull-up (PUPDR bits 27:26 = 01). Active low — reads 0
when pressed, 1 when released. MODER left at reset default (00 = input).

## Peripherals Used
- **RCC** — PLL at 168MHz, PLLQ=7 for 48MHz USB, GPIOA/GPIOC (AHB1), OTG_FS (AHB2), USART2 (APB1)
- **FLASH** — 5 wait states, instruction cache, data cache, prefetch
- **GPIO** — PC13 input pull-up (button), PA11 AF10 (D−), PA12 AF10 (D+), PA2 AF7 (USART2 TX), PA3 AF7 (USART2 RX)
- **OTG_FS** — full-speed USB device, IRQ67
- **SysTick** — 1ms tick for TinyUSB timing
- **USART2** — 9600 baud debug output, BRR = 42000000/9600 = 4375
- **TinyUSB** — USB stack, HID keyboard class, report handling

## Wiring

| Pin | Connect To |
|-----|------------|
| PA11 | USB D− |
| PA12 | USB D+ |
| PC13 | Onboard blue button (no wiring needed) |
| PA2 | Onboard ST-Link (USART2 TX) |
| PA3 | Onboard ST-Link (USART2 RX) |

Connect a USB cable from the STM32 USB connector (not ST-Link) to
your computer. The board enumerates as a HID keyboard. Press the
blue button to type 'H'.

## Note
USB enumeration untested — no hardware available to connect PA11/PA12.
USART2 debug output confirmed working. Logic verified correct.

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