#include "stm32f0xx.h"
#include <stdint.h>
#include <stdlib.h>
#include "lcd.h"

struct{
    int x1;
    int y1;
    int x2;
    int y2;
    int flag;
    int duration;
    int color;
} tiles[] = {{1,-76,29,0,0,-20, RED},
        {31,-20,59,0,0, -20, YELLOW},
        {61,-20,89,0,0, -20, BLUE},
        {91,-20,119,0,0, -20, GREEN},
        {121,-20,149,0,0, -20, CYAN},
        {151,-20,179,0,0, -20, MAGENTA},
        {181,-20,209,0,0, -20, BRRED},
        {211,-20,239,0,0, -20, BROWN},
        {1,-20,29,0,0, -20, RED},
        {31,-20,59,0,0, -20, YELLOW},
        {61,-20,89,0,0, -20, BLUE},
        {91,-20,119,0,0, -20, GREEN},
        {121,-20,149,0,0, -20, CYAN},
        {151,-20,179,0,0, -20, MAGENTA},
        {181,-20,209,0,0, -20, BRRED},
        {211,-20,239,0,0, -20, BROWN},
};


void set_flag(int n){
    tiles[n].flag = 1;
}

void set_next(int tile_num, int duration){
    tiles[tile_num].y1 = -duration;
    tiles[tile_num].duration = duration;
}

void clear_flag(int n){
    tiles[n].flag = 0;
}

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}


extern const Picture background; // A 240x320 background image


void move_tiles(int tile_num){

    if (tiles[tile_num].flag == 1){
           LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y1, tiles[tile_num].x2, tiles[tile_num].y1+1, WHITE);

           if(tiles[tile_num].x1 == 1){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, tiles[tile_num].color);
           }
           else if(tiles[tile_num].x1 == 31){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, tiles[tile_num].color);
           }
           else if(tiles[tile_num].x1 == 61){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, tiles[tile_num].color);
           }
           else if(tiles[tile_num].x1 == 91){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, tiles[tile_num].color);
           }
           else if(tiles[tile_num].x1 == 121){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, tiles[tile_num].color);
           }
           else if(tiles[tile_num].x1 == 151){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, tiles[tile_num].color);
           }
           else if(tiles[tile_num].x1 == 181){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, tiles[tile_num].color);
           }
           else{
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, tiles[tile_num].color);
           }
    tiles[tile_num].y1 += 2;
    tiles[tile_num].y2 += 2;
    }

    if (tiles[tile_num].y1 > 270){
        LCD_DrawFillRectangle(tiles[tile_num].x1, 290, tiles[tile_num].x2, 290, BLACK);
    }

    if (tiles[tile_num].y2 >= 318){
        LCD_DrawFillRectangle(tiles[tile_num].x1, 318, tiles[tile_num].x2, 318, BLACK);
        if (tile_num == 0) tiles[tile_num].color = RED;
        if (tile_num == 1) tiles[tile_num].color = YELLOW;
        if (tile_num == 2) tiles[tile_num].color = BLUE;
        if (tile_num == 3) tiles[tile_num].color = GREEN;
        if (tile_num == 4) tiles[tile_num].color = CYAN;
        if (tile_num == 5) tiles[tile_num].color = MAGENTA;
        if (tile_num == 6) tiles[tile_num].color = BRRED;
        if (tile_num == 7) tiles[tile_num].color = BROWN;
        if (tile_num == 8) tiles[tile_num].color = RED;
        if (tile_num == 9) tiles[tile_num].color = YELLOW;
        if (tile_num == 10) tiles[tile_num].color = BLUE;
        if (tile_num == 11) tiles[tile_num].color = GREEN;
        if (tile_num == 12) tiles[tile_num].color = CYAN;
        if (tile_num == 13) tiles[tile_num].color = MAGENTA;
        if (tile_num == 14) tiles[tile_num].color = BRRED;
        if (tile_num == 15) tiles[tile_num].color = BROWN;
    }


    if (tiles[tile_num].y1 == 320){
        tiles[tile_num].y1 = -20;
        tiles[tile_num].y2 = 0;
        clear_flag(tile_num);
    }
}

//check tiles are in the range
int check_range(int n){
    if(tiles[n].y2 > 290 && tiles[n].y1 < 320){

        return 1;
    }
    else
        return 0;
}


int change_color(int n){
    tiles[n].color = WHITE;
}
void init_tim2(int n) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = (48 - 1);
    TIM2->ARR = (n - 1);
    TIM2->CR1 |= TIM_CR1_ARPE;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;

    NVIC->ISER[0] |= 1<<TIM2_IRQn;

    NVIC_SetPriority(TIM2_IRQn, 3);
}

void init_tim3(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 1000-1;
    TIM3->ARR = 4800-1;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<16;
}

void init_tim7(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 25;
    TIM7->ARR = 480000-1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<18;
    NVIC_SetPriority(TIM7_IRQn,3);
}

void init_tim14(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->PSC = 100-1;
    TIM14->ARR = 480000-1;
    TIM14->DIER |= TIM_DIER_UIE;
    TIM14->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<19;
    NVIC_SetPriority(TIM14_IRQn,3);
}

void init_tim15(void){
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 500-1;
    TIM15->ARR = 480000-1;
    TIM15->DIER |= TIM_DIER_UIE;
    TIM15->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<20;
    NVIC_SetPriority(TIM15_IRQn,3);
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

//PC7 8 9 PA8 9 10 11 12
void setup_buttons(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR7_1;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR8_1;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR9_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR11_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR12_1;

}

void init_spi1() {
    // PB13  SPI1_SCK
    // PC2  SPI1_MISO (AF1)
    // PB15  SPI1_MOSI
    // PB12 SPI1_NSS
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
    SPI2->CR2 = SPI_CR2_DS_3 | SPI_CR2_DS_0 | SPI_CR2_SSOE | SPI_CR2_NSSP;
    SPI2->CR1 |= SPI_CR1_MSTR;
    SPI2->CR1 |= SPI_CR1_SPE;

}

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

void init_dac(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 3<<(4*2);

    RCC->APB1ENR |= RCC_APB1ENR_DACEN; // enable clock to DAC
    DAC->CR &= ~(DAC_CR_TSEL1); // tim6 trgo
    DAC->CR |= DAC_CR_TEN1; // trigger enable
    DAC->CR |= DAC_CR_EN1; // dac enable
}
