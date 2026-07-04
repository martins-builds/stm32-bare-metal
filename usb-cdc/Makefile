TARGET = main
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

TUSB = lib/tinyusb/src

C_SOURCES = src/main.c \
            src/startup.c \
            src/usb_descriptors.c \
            $(TUSB)/tusb.c \
            $(TUSB)/device/usbd.c \
            $(TUSB)/device/usbd_control.c \
            $(TUSB)/class/cdc/cdc_device.c \
            $(TUSB)/portable/synopsys/dwc2/dcd_dwc2.c \
            $(TUSB)/dwc2_common.c

INCLUDES = -Isrc \
           -I$(TUSB) \
           -I$(TUSB)/common \
           -I$(TUSB)/device \
           -I$(TUSB)/portable/synopsys/dwc2

CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
         -O0 -g -Wall \
         -DSTM32F446xx \
         -DCFG_TUSB_MCU=OPT_MCU_STM32F4 \
         $(INCLUDES)

LDFLAGS = -T STM32F446RE.ld \
          -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
          --specs=nosys.specs -nostartfiles \
          -Wl,--gc-sections

all: $(TARGET).bin

$(TARGET).elf: $(C_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -f *.elf *.bin *.map