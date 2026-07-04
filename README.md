# STM32 Bare-Metal

Bare-metal embedded systems projects for the STM32F446RE (Nucleo-64).
No HAL, no CubeIDE, no abstractions. Direct register manipulation only.

## Projects

| Project | Description | Peripherals |
|---------|-------------|-------------|
| [blink](./blink/) | LED blink with busy-wait delay | RCC, GPIO |
| [gpio-input](./gpio-input/) | Button interrupt toggles LED | RCC, GPIO, SYSCFG, EXTI, NVIC |
| [systick](./systick) | Precise 1ms blink using SysTick timer | RCC, GPIO, SysTick |
| [uart](./uart/) | USART2 serial transmit and receive | RCC, GPIO, USART2, SysTick |
| [PLL](./PLL/) | USART2 serial transmit and receive at 180MHz via PLL | RCC, FLASH, GPIO, USART2, SysTick |
| [pwm-servo](./pwm-servo/) | Servo control via TIM2 PWM at 50Hz | RCC, FLASH, GPIO, TIM2, USART2, SysTick |
| [I2C-LCD](./I2C-LCD/) | 16x2 LCD over I2C via PCF8574 backpack in 4-bit mode | RCC, FLASH, GPIO, I2C1, USART2, SysTick, DWT |
| [ADC-potentiometer](./ADC-potentiometer/) | 12-bit ADC potentiometer reading converted to millivolts | RCC, FLASH, GPIO, ADC1, USART2, SysTick |
| [dma-adc](./dma-adc/) | ADC continuous conversion via DMA2 to memory, printed over USART2 | RCC, FLASH, GPIO, ADC1, DMA2, USART2, SysTick |
| [spi](./spi/) | MFRC522 RFID card UID reader over SPI1 | RCC, FLASH, GPIO, SPI1, USART2, SysTick |
| [freertos-capstone](./freertos-capstone/) | ADC + RFID + I2C LCD under FreeRTOS, 3 tasks with queues and mutex | RCC, FLASH, GPIO, ADC1, I2C1, SPI1, USART2, FreeRTOS |
| [usb-cdc](./usb-cdc/) | Virtual serial port over USB using TinyUSB CDC class | RCC, FLASH, GPIO, OTG_FS, SysTick, TinyUSB |
| [usb-hid](./usb-hid/) | USB HID keyboard — onboard button sends keypress to host | RCC, FLASH, GPIO, OTG_FS, SysTick, TinyUSB |


## Toolchain
- arm-none-eabi-gcc
- st-flash
- macOS + Homebrew
- FreeRTOS kernel

## Key Differences from AVR
- Clock gating required via RCC before any peripheral works
- Each GPIO pin uses 2 bits in MODER (not 1 like AVR)
- Alternate function register (AFRL/AFRH) needed for UART, SPI, I2C
- Linker script and startup file written manually
- 32-bit registers, Cortex-M4 architecture with NVIC interrupt controller
- SYSCFG required to route GPIO pins to EXTI lines
- SysTick is a Cortex-M4 core peripheral — no RCC gating needed

## Flash Any Project
cd <project-folder>
make flash