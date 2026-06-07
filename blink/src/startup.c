#include <stdint.h>

// defined by linker script
extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;

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
    (void*)&_estack,     // initial stack pointer
    Reset_Handler,       // reset handler
    Default_Handler,     // NMI
    Default_Handler,     // HardFault
    Default_Handler,     // MemManage
    Default_Handler,     // BusFault
    Default_Handler,     // UsageFault
};