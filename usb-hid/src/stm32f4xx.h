#ifndef STM32F4XX_H
#define STM32F4XX_H

#include <stdint.h>

// OTG_FS base address
#define USB_OTG_FS_PERIPH_BASE      0x50000000UL
#define USB_OTG_FS_MAX_IN_ENDPOINTS 4
#define USB_OTG_HS_MAX_IN_ENDPOINTS 6

// System core clock
extern uint32_t SystemCoreClock;

// IRQ numbers
typedef enum {
    OTG_FS_IRQn = 67,
    OTG_HS_IRQn = 77,
} IRQn_Type;

// NVIC registers
#define NVIC_BASE           0xE000E100UL
#define NVIC_ISER_BASE      ((volatile uint32_t*)(NVIC_BASE + 0x000))
#define NVIC_ICER_BASE      ((volatile uint32_t*)(NVIC_BASE + 0x080))

static inline void NVIC_EnableIRQ(IRQn_Type irqn){
    NVIC_ISER_BASE[irqn / 32] = (1UL << (irqn % 32));
}

static inline void NVIC_DisableIRQ(IRQn_Type irqn){
    NVIC_ICER_BASE[irqn / 32] = (1UL << (irqn % 32));
}

// NOP
static inline void __NOP(void){ __asm volatile("nop"); }

#endif