#include <stdint.h>

// defined by linker script
extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;

void SysTick_Handler(void);
void OTG_FS_IRQHandler(void);

void main(void);

void Reset_Handler(void) {
    // copy .data from flash to RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    // zero .bss
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;

    // call main
    main();
    while(1);
}

void Default_Handler(void) { while(1); }

// vector table
__attribute__((section(".isr_vector")))
void (*const vector_table[])(void) = {
    (void*)&_estack,          // 0  initial stack pointer
    Reset_Handler,            // 1  reset
    Default_Handler,          // 2  NMI
    Default_Handler,          // 3  HardFault
    Default_Handler,          // 4  MemManage
    Default_Handler,          // 5  BusFault
    Default_Handler,          // 6  UsageFault
    Default_Handler,          // 7
    Default_Handler,          // 8
    Default_Handler,          // 9
    Default_Handler,          // 10
    Default_Handler,          // 11
    Default_Handler,          // 12
    Default_Handler,          // 13
    Default_Handler,          // 14
    SysTick_Handler,          // 15 - SysTick
    Default_Handler,          // 16 — IRQ0
    Default_Handler,          // 17 — IRQ1
    Default_Handler,          // 18
    Default_Handler,          // 19
    Default_Handler,          // 20
    Default_Handler,          // 21
    Default_Handler,          // 22
    Default_Handler,          // 23
    Default_Handler,          // 24
    Default_Handler,          // 25
    Default_Handler,          // 26
    Default_Handler,          // 27
    Default_Handler,          // 28
    Default_Handler,          // 29
    Default_Handler,          // 30
    Default_Handler,          // 31
    Default_Handler,          // 32
    Default_Handler,          // 33
    Default_Handler,          // 34
    Default_Handler,          // 35
    Default_Handler,          // 36
    Default_Handler,          // 37
    Default_Handler,          // 38
    Default_Handler,          // 39
    Default_Handler,          // 40
    Default_Handler,          // 41
    Default_Handler,          // 42
    Default_Handler,          // 43
    Default_Handler,          // 44
    Default_Handler,          // 45
    Default_Handler,          // 46
    Default_Handler,          // 47
    Default_Handler,          // 48
    Default_Handler,          // 49
    Default_Handler,          // 50
    Default_Handler,          // 51
    Default_Handler,          // 52
    Default_Handler,          // 53
    Default_Handler,          // 54
    Default_Handler,          // 55
    Default_Handler,     // 56 — IRQ40
    Default_Handler,          // 57
    Default_Handler,          // 58
    Default_Handler,          // 59
    Default_Handler,          // 60
    Default_Handler,          // 61
    Default_Handler,          // 62
    Default_Handler,          // 63
    Default_Handler,          // 64
    Default_Handler,          // 65
    Default_Handler,          // 66
    Default_Handler,          // 67
    Default_Handler,          // 68
    Default_Handler,          // 69
    Default_Handler,          // 70
    Default_Handler,          // 71
    Default_Handler,          // 72
    Default_Handler,          // 73
    Default_Handler,          // 74
    Default_Handler,          // 75
    Default_Handler,          // 76
    Default_Handler,          // 77
    Default_Handler,          // 78
    Default_Handler,          // 79
    Default_Handler,          // 80
    Default_Handler,          // 81
    Default_Handler,          // 82
    OTG_FS_IRQHandler,        // 83 — IRQ67 OTG_FS
};