#include <stdint.h>

#define RCC_BASE    0x40023800
#define GPIOA_BASE  0x40020000

#define RCC_AHB1ENR  (*(volatile uint32_t*)(RCC_BASE   + 0x30))
#define GPIOA_MODER  (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR    (*(volatile uint32_t*)(GPIOA_BASE + 0x14))

void delay(volatile uint32_t count){
    while(count--);
}

__attribute__((used)) void main(void){
    // enable GPIOA clock
    RCC_AHB1ENR |= (1 << 0);

    // PA5 as output (bits 11:10 = 01)
    GPIOA_MODER |= (1 << 10);
    GPIOA_MODER &= ~(1 << 11);

    while(1){
        GPIOA_ODR |= (1 << 5);   // LED on
        delay(500000);
        GPIOA_ODR &= ~(1 << 5);  // LED off
        delay(500000);
    }
}