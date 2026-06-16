#include <stdint.h>

#define SYST_CSR  (*(volatile uint32_t*)0XE000E010)
#define SYST_RVR  (*(volatile uint32_t*)0XE000E014)
#define SYST_CVR  (*(volatile uint32_t*)0XE000E018)

#define RCC_BASE    0x40023800
#define GPIOA_BASE  0x40020000
#define USART2_BASE  0x40004400
#define TIM2_BASE   0x40000000

//PINS
#define RCC_AHB1ENR  (*(volatile uint32_t*)(RCC_BASE   + 0x30))
#define GPIOA_MODER  (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR    (*(volatile uint32_t*)(GPIOA_BASE + 0x14))
#define GPIOA_AFRL   (*(volatile uint32_t*)(GPIOA_BASE + 0x20))

//USART
#define RCC_APB1ENR  (*(volatile uint32_t*)(RCC_BASE    + 0x40))
#define USART_SR     (*(volatile uint32_t*)(USART2_BASE + 0x00))
#define USART_DR     (*(volatile uint32_t*)(USART2_BASE + 0x04))
#define USART_CR1    (*(volatile uint32_t*)(USART2_BASE + 0x0C))
#define USART_BRR    (*(volatile uint32_t*)(USART2_BASE + 0x08))

//PLL
#define RCC_CR       (*(volatile uint32_t*)(RCC_BASE + 0x00))
#define RCC_PLLCFGR  (*(volatile uint32_t*)(RCC_BASE + 0x04))
#define RCC_CFGR     (*(volatile uint32_t*)(RCC_BASE + 0x08))
#define FLASH_ACR    (*(volatile uint32_t*)(0x40023C00 + 0x00))

//TIMER
#define TIM2_CR1    (*(volatile uint32_t*)(TIM2_BASE + 0x00))
#define TIM2_EGR    (*(volatile uint32_t*)(TIM2_BASE + 0x14))
#define TIM2_CCMR1  (*(volatile uint32_t*)(TIM2_BASE + 0x18))
#define TIM2_CCER   (*(volatile uint32_t*)(TIM2_BASE + 0x20))
#define TIM2_ARR    (*(volatile uint32_t*)(TIM2_BASE + 0x2C))
#define TIM2_PSC    (*(volatile uint32_t*)(TIM2_BASE + 0x28))
#define TIM2_CCR1   (*(volatile uint32_t*)(TIM2_BASE + 0x34))

//start of SysTick
volatile uint32_t tick = 0;

void systick_init(void){
    uint32_t reload_value = 180000000 / 1000 - 1;
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

void clock_init(){
    FLASH_ACR |= (5 << 0);
    FLASH_ACR |= (1 << 8) | (1 << 9) | (1 << 10);
    RCC_CR |= (1 << 16);  // enable HSE
    
    // wait for HSE with timeout
    uint32_t timeout = 100000;
    while(!(RCC_CR & (1 << 17)) && timeout--);
    
    if(!timeout) return;  // HSE failed, stay on HSI
    RCC_PLLCFGR = (1 << 22) | (8 << 0) | (360 << 6) | (0 << 16);
    RCC_CR |= (1 << 24);
    timeout = 100000;
    while(!(RCC_CR & (1 << 25)) && timeout--);
    if(!timeout) return;

    RCC_CFGR = (0 << 4) | (5 << 10) | (4 << 13);
    RCC_CFGR |= (2 << 0);
    timeout = 100000;
    while(!(RCC_CFGR & (2 << 2)) && timeout--);
}

//TIMER-FUNCTIONS
void timer_init(){
    TIM2_PSC = 89;
    TIM2_ARR = 19999;
    TIM2_CCR1 = 3000;
    TIM2_CCMR1 |= (6 << 4);
    TIM2_CCER |= (1 << 0);
    TIM2_EGR |= (1 << 0);
    TIM2_CR1 |= (1 << 7) | (1 << 0);
}
void servo_set_angle(uint8_t angle){
    TIM2_CCR1 = 1000 + (angle * 1000UL / 180);
}

__attribute__((used)) void main(void){
    clock_init();
    systick_init();

    RCC_AHB1ENR |= (1 << 0);
    RCC_APB1ENR |= (1 << 0);  // TIM2EN

    // PA2 bits 5:4 = 10
    GPIOA_MODER |= (1 << 5);
    GPIOA_MODER &= ~(1 << 4);
    // PA3 bits 7:6 = 10
    GPIOA_MODER |= (1 << 7);
    GPIOA_MODER &= ~(1 << 6);

    GPIOA_AFRL |= (7 << 8);   // PA2 = AF7
    GPIOA_AFRL |= (7 << 12);  // PA3 = AF7

    usart_init();

    // PA0 alternate function mode
    GPIOA_MODER |= (1 << 1);
    GPIOA_MODER &= ~(1 << 0);
    GPIOA_AFRL |= (1 << 0);   // PA0 = AF1

    timer_init();

    while(1){
        usart_print("Hello from STM32\r\n");
        delay_ms(1000);
        servo_set_angle(0);
        delay_ms(1000);
        servo_set_angle(90);
        delay_ms(1000);
        servo_set_angle(180);
        delay_ms(1000);
    }
}