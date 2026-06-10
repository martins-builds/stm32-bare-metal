#include <stdint.h>

#define SYST_CSR  (*(volatile uint32_t*)0XE000E010)
#define SYST_RVR  (*(volatile uint32_t*)0XE000E014)
#define SYST_CVR  (*(volatile uint32_t*)0XE000E018)

#define RCC_BASE    0x40023800
#define GPIOA_BASE  0x40020000

#define RCC_AHB1ENR  (*(volatile uint32_t*)(RCC_BASE   + 0x30))
#define GPIOA_MODER  (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR    (*(volatile uint32_t*)(GPIOA_BASE + 0x14))

volatile uint32_t tick = 0;

void systick_init(void){
    uint32_t reload_value = 16000000 / 1000 - 1;
    SYST_RVR = reload_value;
    SYST_CVR = 0;
    SYST_CSR = (1 << 0) | (1 << 1) | (1 << 2);
}
void SysTick_Handler(void){
    tick++;
}
void delay_ms(uint32_t ms){
    uint32_t start = tick;
    while((tick - start) < ms);
}


__attribute__((used)) void main(void){
    systick_init();

    // enable GPIOA clock
    RCC_AHB1ENR |= (1 << 0);

    // PA5 as output (bits 11:10 = 01)
    GPIOA_MODER |= (1 << 10);
    GPIOA_MODER &= ~(1 << 11);

    while (1)
    {
        GPIOA_ODR |= (1 << 5);   // LED on
        delay_ms(500);
        GPIOA_ODR &= ~(1 << 5);   // LED off
        delay_ms(500);
    }
}