CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O0 -g -nostdlib

SRCS = src/main.c src/startup.c
LDSCRIPT = STM32F446RE.ld

all: main.bin

main.elf: $(SRCS)
	$(CC) $(CFLAGS) -T$(LDSCRIPT) -o main.elf $(SRCS) -Wl,-Map=main.map

main.bin: main.elf
	arm-none-eabi-objcopy -O binary main.elf main.bin

flash: main.bin
	st-flash write main.bin 0x08000000

clean:
	rm -f main.elf main.bin