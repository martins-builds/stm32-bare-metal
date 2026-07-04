#include <stdint.h>
#include "tusb.h"

#define RCC_BASE    0x40023800
#define GPIOA_BASE  0x40020000
#define GPIOB_BASE  0x40020400
#define OTG_FS      0x50000000

//PLL
#define RCC_CR       (*(volatile uint32_t*)(RCC_BASE + 0x00))
#define RCC_PLLCFGR  (*(volatile uint32_t*)(RCC_BASE + 0x04))
#define RCC_CFGR     (*(volatile uint32_t*)(RCC_BASE + 0x08))
#define FLASH_ACR    (*(volatile uint32_t*)(0x40023C00 + 0x00))

//PINS
//port A
#define RCC_AHB1ENR  (*(volatile uint32_t*)(RCC_BASE   + 0x30))
#define GPIOA_MODER  (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR    (*(volatile uint32_t*)(GPIOA_BASE + 0x14))
#define GPIOA_AFRL   (*(volatile uint32_t*)(GPIOA_BASE + 0x20))
#define GPIOA_OTYPER (*(volatile uint32_t*)(GPIOA_BASE + 0x04))

//port B
#define GPIOB_MODER   (*(volatile uint32_t*)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER  (*(volatile uint32_t*)(GPIOB_BASE + 0x04))
#define GPIOB_OSPEEDR (*(volatile uint32_t*)(GPIOB_BASE + 0x08))
#define GPIOB_AFRH    (*(volatile uint32_t*)(GPIOB_BASE + 0x24))

//OTG
#define RCC_AHB2ENR   (*(volatile uint32_t*)(RCC_BASE  + 0x34))

#define OTG_GAHBCFG   (*(volatile uint32_t*)(OTG_FS  + 0x008))
#define OTG_GINTMSK   (*(volatile uint32_t*)(OTG_FS  + 0x018))
#define OTG_GINTSTS   (*(volatile uint32_t*)(OTG_FS  + 0x014))
#define OTG_GUSBCFG   (*(volatile uint32_t*)(OTG_FS  + 0x00C))
#define OTG_DCFG      (*(volatile uint32_t*)(OTG_FS  + 0x800))
#define OTG_DTHRCTL   (*(volatile uint32_t*)(OTG_FS  + 0x0830))
#define OTG_DCTL      (*(volatile uint32_t*)(OTG_FS  + 0x804))
#define OTG_DSTS      (*(volatile uint32_t*)(OTG_FS  + 0x808))

void clock_init(){
    FLASH_ACR |= (5 << 0);
    FLASH_ACR |= (1 << 8) | (1 << 9) | (1 << 10);
    RCC_CR |= (1 << 16);
    while(!(RCC_CR & (1 << 17)));  // wait until bit 17 becomes 1
    RCC_PLLCFGR = (1 << 22) | (8 << 0) | (336 << 6) | (0 << 16) | (7 << 24);
    RCC_CR |= (1 << 24);
    while(!(RCC_CR & (1 << 25)));
    RCC_CFGR = (0 << 4) | (5 << 10) | (4 << 13);
    RCC_CFGR |= (2 << 0);
    while(!(RCC_CFGR & (2 << 2)));
}

_attribute__((used)) void main(void){

    while(1);
}