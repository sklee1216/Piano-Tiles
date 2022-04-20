#include "stm32f0xx.h"
#include "lcd.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void init_tiles(void);
void nano_wait(unsigned int n);
void move_tiles(int tile_num);

int score = 0;
char score_string[10];
char conc_string[10];

void init_lcd_spi(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~0x30c30000;
    GPIOB->MODER |= 0x10410000;
    GPIOB->ODR |= 0x4900;

    GPIOB->MODER &= ~0xc0;
    GPIOB->MODER |= 0x80;

    GPIOB->MODER &= ~0xc00;
    GPIOB->MODER |= 0x800;

    GPIOB->AFR[0] &= ~0xf0f000;

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 &= ~SPI_CR1_SPE;
    SPI1->CR1 &= ~0x3f;
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void init_tim7(void){

    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 0;
    TIM7->ARR = 480-1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<18;

}

void init_tim6(void){

    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 0;
    TIM6->ARR = 480-1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<17;

}

void TIM6_DAC_IRQHandler(void){
    int start = (GPIOA->IDR) & 1;
    if (start == 1){
        init_tim7();
        TIM6->CR1 &= ~TIM_CR1_CEN;
    }
    }

void init_tim15(void){
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 0;
    TIM15->ARR = 4800-1;
    TIM15->DIER |= TIM_DIER_UIE;
    TIM15->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<20;
}

void TIM7_IRQHandler(void){

    TIM7->SR &= ~(1<<0);

    int r = rand() % 10;
    move_tiles(r);
    //nano_wait(100000);
    //move_tiles(r);

}



#define STEP22
#if defined(STEP22)
void TIM15_IRQHandler(void){
    TIM15->SR &= ~(1<<0);
    int a = (GPIOA->IDR) & 1;

    if (a == 1){
        spi1_display1("Hello");
    }

}
#endif


void setup_buttons(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0xffff;
    GPIOC->MODER |= 0x55 << (4*2);
    GPIOC->OTYPER &= ~0xf0;
    GPIOC->OTYPER |= 0xf0;
    GPIOC->PUPDR &= ~0xff;
    GPIOC->PUPDR |= 0x55;

    RCC->AHBENR |= 1<<17;
    GPIOA->PUPDR |= 1<<1;
}

//void basic_drawing(void);
//void move_ball(void);

void init_spi1() {
    // PB13  SPI1_SCK
    // PC2  SPI1_MISO (AF1)
    // PB15  SPI1_MOSI
    // PB12 SPI1_NSS
    RCC->AHBENR |= 1<<19;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER |= GPIO_MODER_MODER13_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER13_0;
    GPIOB->MODER |= GPIO_MODER_MODER15_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER15_0;
    GPIOB->MODER |= GPIO_MODER_MODER12_1;
    GPIOB->MODER &= ~GPIO_MODER_MODER12_0;


    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    SPI2->CR1 &= ~SPI_CR1_SPE;

    SPI2->CR1 |= SPI_CR1_BR;

    SPI2->CR2 = SPI_CR2_DS_3 | SPI_CR2_DS_0 | SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_TXDMAEN;

    SPI2->CR1 |= SPI_CR1_MSTR;

    SPI2->CR1 |= SPI_CR1_SPE;

}

void spi_cmd(unsigned int data) {
    while(!(SPI2->SR & SPI_SR_TXE)) {}
    SPI2->DR = data;
}
void spi_data(unsigned int data) {
    spi_cmd(data | 0x200);
}
void spi1_init_oled() {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    spi_cmd(0x01);
    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}
void spi1_display1(const char *string) {
    spi_cmd(0x02);
    while(*string != '\0') {
        spi_data(*string);
        string++;
    }
}
void spi1_display2(const char *string) {
    spi_cmd(0xc0);
    while(*string != '\0') {
        spi_data(*string);
        string++;
    }
}




int main(void)
{
    setup_buttons();
    LCD_Setup(); // this will call init_lcd_spi()
    init_tiles();



    init_spi1();
    spi1_init_oled();
    spi1_display1("Press any button to start");
    spi1_display2("Fuck this class");

    init_tim15();

    //if (start == 1)
      //  init_tim7();*/

}

