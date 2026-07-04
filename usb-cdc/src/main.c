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
#define GPIOA_AFRH   (*(volatile uint32_t*)(GPIOA_BASE + 0x24))
#define GPIOA_OTYPER (*(volatile uint32_t*)(GPIOA_BASE + 0x04))

//port B
#define GPIOB_MODER   (*(volatile uint32_t*)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER  (*(volatile uint32_t*)(GPIOB_BASE + 0x04))
#define GPIOB_OSPEEDR (*(volatile uint32_t*)(GPIOB_BASE + 0x08))
#define GPIOB_AFRH    (*(volatile uint32_t*)(GPIOB_BASE + 0x24))

//USART
#define RCC_APB1ENR  (*(volatile uint32_t*)(RCC_BASE    + 0x40))
#define USART_SR     (*(volatile uint32_t*)(USART2_BASE + 0x00))
#define USART_DR     (*(volatile uint32_t*)(USART2_BASE + 0x04))
#define USART_CR1    (*(volatile uint32_t*)(USART2_BASE + 0x0C))
#define USART_BRR    (*(volatile uint32_t*)(USART2_BASE + 0x08))

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

//clock function for USB CDC
void clock_init(void){
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

//start of usart2
void usart_init(void){
    RCC_APB1ENR |= (1 << 17);  // USART2EN
    USART_BRR = 45000000 / 9600;  // = 4687
    USART_CR1 = (1 << 13) | (1 << 3) | (1 << 2);  // UE, TE, RE
}
void usart_send(char data){
    while(!(USART_SR & (1 << 7)));  // wait until TXE is set
    USART_DR = data;
}
char usart_receive(void){
    while(!(USART_SR & (1 << 5)));  // wait until RXNE is set
    return USART_DR;
}
void usart_print(const char *str){
    while (*str) {
        usart_send(*str++);
    }
}
void usart_print_number(uint16_t num){
    char buf[6];
    uint8_t i = 0;
    
    if (num == 0){
        usart_send('0');
        return;
    }
    while (num > 0){
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    // digits are reversed, print backwards
    while (i > 0){
        usart_send(buf[--i]);
    }
}
void usart_print_hex(uint8_t val){
    char hex[] = "0123456789ABCDEF";
    usart_send(hex[val >> 4]);
    usart_send(hex[val & 0x0F]);
}


_attribute__((used)) void main(void){
    clock_init();

    // enable GPIOA clock
    RCC_AHB1ENR |= (1 << 0);

    // PA2 bits 5:4 = 10
    GPIOA_MODER |= (1 << 5);
    GPIOA_MODER &= ~(1 << 4);
    // PA3 bits 7:6 = 10
    GPIOA_MODER |= (1 << 7);
    GPIOA_MODER &= ~(1 << 6);

    GPIOA_AFRL |= (7 << 8);   // PA2 = AF7
    GPIOA_AFRL |= (7 << 12);  // PA3 = AF7

    GPIOA_AFRH |= (10 << 12); // PA11 = AF10
    GPIOA_AFRH |= (10 << 16); // PA12 = AF10
    
    usart_init();

    while(1);
}