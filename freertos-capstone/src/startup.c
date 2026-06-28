#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;

void main(void);
void Reset_Handler(void);
void Default_Handler(void);

// FreeRTOS handlers
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void Reset_Handler(void) {
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;
    main();
    while(1);
}

void Default_Handler(void) { while(1); }

__attribute__((section(".isr_vector")))
void (*const vector_table[])(void) = {
    (void*)&_estack,
    Reset_Handler,
    Default_Handler,          // NMI
    Default_Handler,          // HardFault
    Default_Handler,          // MemManage
    Default_Handler,          // BusFault
    Default_Handler,          // UsageFault
    Default_Handler,          // 7
    Default_Handler,          // 8
    Default_Handler,          // 9
    Default_Handler,          // 10
    SVC_Handler,              // 11 SVC — FreeRTOS
    Default_Handler,          // 12
    Default_Handler,          // 13
    PendSV_Handler,           // 14 PendSV — FreeRTOS
    SysTick_Handler,          // 15 SysTick — FreeRTOS
    Default_Handler,          // IRQ0
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,          // IRQ40
};