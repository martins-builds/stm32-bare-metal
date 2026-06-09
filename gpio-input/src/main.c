#include <stdint.h>

#define NVIC_ISER1  (*(volatile uint32_t*)0xE000E104)

#define RCC_BASE    0x40023800
#define GPIOA_BASE  0x40020000
#define GPIOC_BASE  0x40020800
#define SYSCFG_BASE 0x40013800
#define EXTI_BASE   0x40013C00

#define RCC_AHB1ENR  (*(volatile uint32_t*)(RCC_BASE   + 0x30))
#define GPIOA_MODER  (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR    (*(volatile uint32_t*)(GPIOA_BASE + 0x14))

#define RCC_APB2ENR  (*(volatile uint32_t*)(RCC_BASE   + 0x44))

#define GPIOC_MODER  (*(volatile uint32_t*)(GPIOC_BASE + 0x00))
#define GPIOC_PUPDR  (*(volatile uint32_t*)(GPIOC_BASE + 0x0C))

#define SYSCFG_EXTICR4  (*(volatile uint32_t*)(SYSCFG_BASE + 0x14))
#define EXTI_IMR   (*(volatile uint32_t*)(EXTI_BASE + 0x00))
#define EXTI_FTSR  (*(volatile uint32_t*)(EXTI_BASE + 0x0C))
#define EXTI_PR    (*(volatile uint32_t*)(EXTI_BASE + 0x14))

void EXTI15_10_IRQHandler(void) {
    // clear the pending flag first
    EXTI_PR |= (1 << 13);
    GPIOA_ODR ^= (1 << 5);
}

void delay(volatile uint32_t count){
    while(count--);
}

__attribute__((used)) void main(void){
    NVIC_ISER1 |= (1 << 8);  // enable EXTI15_10 interrupt
    RCC_APB2ENR |= (1 << 14);

    // enable GPIOA clock
    RCC_AHB1ENR |= (1 << 0);
    // PA5 as output (bits 11:10 = 01)
    GPIOA_MODER |= (1 << 10);
    GPIOA_MODER &= ~(1 << 11);

    // GPIOC clock
    RCC_AHB1ENR |= (1 << 2);  
    // PC13 for input with pull up
    GPIOC_PUPDR |= (1 << 26);
    GPIOC_PUPDR &= ~(1 << 27);

    SYSCFG_EXTICR4 &= ~(1 << 7);
    SYSCFG_EXTICR4 &= ~(1 << 6);
    SYSCFG_EXTICR4 |= (1 << 5);
    SYSCFG_EXTICR4 &= ~(1 << 4);

    EXTI_IMR |= (1 << 13);
    EXTI_FTSR |= (1 << 13);

    while(1);
}