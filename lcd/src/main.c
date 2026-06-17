#include <stdint.h>

#define SYST_CSR  (*(volatile uint32_t*)0XE000E010)
#define SYST_RVR  (*(volatile uint32_t*)0XE000E014)
#define SYST_CVR  (*(volatile uint32_t*)0XE000E018)

#define RCC_BASE     0x40023800
#define GPIOA_BASE   0x40020000
#define USART2_BASE  0x40004400
#define I2C_BASE     0x40005400
#define GPIOB_BASE   0x40020400

//PINS
#define RCC_AHB1ENR  (*(volatile uint32_t*)(RCC_BASE   + 0x30))
#define GPIOA_MODER  (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR    (*(volatile uint32_t*)(GPIOA_BASE + 0x14))
#define GPIOA_AFRL   (*(volatile uint32_t*)(GPIOA_BASE + 0x20))

#define GPIOB_MODER  (*(volatile uint32_t*)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER (*(volatile uint32_t*)(GPIOB_BASE + 0x04))
#define GPIOB_OSPEEDR (*(volatile uint32_t*)(GPIOB_BASE + 0x08))
#define GPIOB_AFRH   (*(volatile uint32_t*)(GPIOB_BASE + 0x24))

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

//I2C
#define I2C_CR1    (*(volatile uint32_t*)(I2C_BASE + 0x00))
#define I2C_CR2    (*(volatile uint32_t*)(I2C_BASE + 0x04))
#define I2C_CCR    (*(volatile uint32_t*)(I2C_BASE + 0x1C))
#define I2C_TRISE  (*(volatile uint32_t*)(I2C_BASE + 0x20))
#define I2C_DR     (*(volatile uint32_t*)(I2C_BASE + 0x10))
#define I2C_SR1    (*(volatile uint32_t*)(I2C_BASE + 0x14))
#define I2C_SR2    (*(volatile uint32_t*)(I2C_BASE + 0x18))
#define I2C_OAR1   (*(volatile uint32_t*)(I2C_BASE + 0x08))

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

//clock function
void clock_init(){
    FLASH_ACR |= (5 << 0);
    FLASH_ACR |= (1 << 8) | (1 << 9) | (1 << 10);
    RCC_CR |= (1 << 16);
    while(!(RCC_CR & (1 << 17)));  // wait until bit 17 becomes 1
    RCC_PLLCFGR = (1 << 22) | (8 << 0) | (360 << 6) | (0 << 16);
    RCC_CR |= (1 << 24);
    while(!(RCC_CR & (1 << 25)));
    RCC_CFGR = (0 << 4) | (5 << 10) | (4 << 13);
    RCC_CFGR |= (2 << 0);
    while(!(RCC_CFGR & (2 << 2)));
}

//I2C function
void i2c_init(){
    // GPIOB clock
    RCC_AHB1ENR |= (1 << 1);
    RCC_APB1ENR |= (1 << 21);

    // PB10 and PB11 alternate function mode (10)
    GPIOB_MODER |= (1 << 21) | (1 << 23);
    GPIOB_MODER &= ~((1 << 20) | (1 << 22));

    // open-drain
    GPIOB_OTYPER |= (1 << 10) | (1 << 11);

    // high speed
    GPIOB_OSPEEDR |= (1 << 21) | (1 << 20) | (1 << 23) | (1 << 22);

    GPIOB_AFRH &= ~(0xF << 8);   // clear PB10's 4 bits
    GPIOB_AFRH |=  (4   << 8);   // set AF4
    GPIOB_AFRH &= ~(0xF << 12);  // clear PB11's 4 bits
    GPIOB_AFRH |=  (4   << 12);  // set AF4

    I2C_CR2 = 45;
    I2C_CCR = 225;
    I2C_TRISE = 46;
    I2C_CR1 |= (1 << 0);
}

void i2c_start(){
    I2C_CR1 |= (1 << 8);
    while(!(I2C_SR1 & (1 << 0)));
}
void i2c_write_addr(uint8_t addr){
    I2C_DR = (addr << 1);
    while(!(I2C_SR1 & (1 << 1)));
    volatile uint32_t tmp = I2C_SR1;
    tmp = I2C_SR2;
    (void)tmp;
}
void i2c_write_byte(uint8_t data){
    I2C_DR = data;
    while(!(I2C_SR1 & (1 << 2)));
}
void i2c_stop(){

    I2C_CR1 |= (1 << 9);
}

__attribute__((used)) void main(void){
    clock_init();
    systick_init();

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

    usart_init();
    
    while(1){
        usart_print("Hello from STM32\r\n");
        delay_ms(1000);
    }
}