#include "stm32f0xx.h"
#include "lcd.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "midiplay.h"
#include <math.h>

void init_tiles(void);
void nano_wait(unsigned int n);
void move_tiles(int tile_num);


float score = 0;
char score_string[10];
char conc_string[10];

#include "stm32f0xx.h"
#include <stdint.h>
#include "midiplay.h"

// The number of simultaneous voices to support.
#define VOICES 1

// An array of "voices".  Each voice can be used to play a different note.
// Each voice has a step size and an offset into the wave table.
struct {
    uint8_t in_use; // 1 or 0
    uint8_t note; // note
    uint8_t chan; //
    uint8_t volume;
    int     step; // step is updated offset
    int     offset; // constant offset
} voice[VOICES];

// We'll use the Timer 6 IRQ to recompute samples and feed those
// samples into the DAC.
void TIM6_DAC_IRQHandler(void)
{
    TIM6->SR = ~TIM_SR_UIF;


    int sample = 0;
    // for loop to cover all 8 voices
    for(int x=0; x < sizeof voice / sizeof voice[0]; x++) {
    // if the note is in use
        if (voice[x].in_use) {
            voice[x].offset += voice[x].step; // creates offset that is "step" sized for next note
            if (voice[x].offset >= N<<16)
                voice[x].offset -= N<<16;
            sample += (wavetable[voice[x].offset>>16] * voice[x].volume) >> 4;
        }
    }
    sample = (sample >> 10) + 2048;
    if (sample > 4095)
        sample = 4095;
    else if (sample < 0)
        sample = 0;
    DAC->DHR12R1 = sample;
}

// Initialize Timer 6 so that it calls TIM6_DAC_IRQHandlerisp1[0] !=
// at exactly RATE times per second.
void init_tim6(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = (480 - 1);
    TIM6->ARR = (100000/RATE - 1);
    TIM6->CR2 &= ~0x70;
    TIM6->CR2 |= 0x20;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;

    NVIC->ISER[0] |= 1<<TIM6_DAC_IRQn;
}

// Initialize the DAC so that it can output analog samples
// on PA4.  Configure it to be triggered by TIM6 TRGO.
void init_dac(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 3<<(4*2);

    RCC->APB1ENR |= RCC_APB1ENR_DACEN; // enable clock to DAC
    DAC->CR &= ~(DAC_CR_TSEL1); // tim6 trgo
    DAC->CR |= DAC_CR_TEN1; // trigger enable
    DAC->CR |= DAC_CR_EN1; // dac enable
}

// Find the voice current playing a note, and turn it off.
void note_off(int time, int chan, int key, int velo)
{
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use && voice[n].note == key) {
            voice[n].in_use = 0; // disable it first...
            voice[n].chan = 0;   // ...then clear its values
            voice[n].note = key;
            voice[n].step = step[key];
            return;
        }
    }
}

int count;
// Find an unused voice, and use it to play a note.
void note_on(int time, int chan, int key, int velo)
{
    count = 0;
    if (velo == 0) {
        note_off(time, chan, key, velo);
        return;
    }
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use == 0) {
            voice[n].note = key;
            voice[n].step = step[key];
            voice[n].offset = 0;
            voice[n].volume = velo;
            voice[n].chan = chan;
            voice[n].in_use = 1;
            return;
        }
    }
}

// An array of structures that say when to turn notes on or off.
struct {
    uint16_t when;
    uint16_t note;
    uint16_t volume;
    uint16_t duration;
    uint16_t track;

} events[] = {
//        {480,84,0x75,76, 1}, {556,84,0x00,0, 1}, {960,84,0x75,48, 2}, {1008,84,0x00,0, 2},
//        {1440,91,0x75,80, 3}, {1520,91,0x00,0, 3}, {1920,91,0x75,76, 4}, {1996,91,0x00,0, 4},
//        {2400,93,0x76,72, 5}, {2472,93,0x00,0, 5}, {2640,94,0x67,80, 6}, {2720,94,0x00,0, 6},
//        {2880,96,0x67,80, 7}, {2960,96,0x00,0, 7}, {3120,93,0x6d,60, 8}, {3180,93,0x00,0, 8},
//        {3360,91,0x79,80, 9}, {3440,91,0x00,0, 7}, {4320,89,0x70,88, 10}, {4408,89,0x00,0, 6},
//        {4800,89,0x73, 84, 11}, {4884,89,0x00, 0, 5}, {5280,88,0x73, 80, 12}, {5360,88,0x00, 0, 4},
//        {5760,91,0x79, 76, 13}, {5836,91,0x00, 0, 3}, {6240,86,0x79, 68, 14}, {6308,86,0x00, 0, 2},
//        {6720,86,0x76, 48, 1}, {6768,86,0x00, 0, 1}, {7200,84,0x76, 52, 7}, {7252,84,0x00, 0, 7},
//        {8160,84,0x73, 76, 6}, {8236,84,0x00,0, 6}, {8640,84,0x74, 48, 5}, {8688,84,0x00, 0, 5},
//        {9120,91,0x76, 80, 4}, {9200,91,0x00, 0, 4}, {9600,91,0x79, 76, 3}, {9676,91,0x00, 0 , 3},
//        {10080,93,0x76, 72, 1}, {10152,93,0x00, 0,1}, {10320,94,0x67, 80, 2}, {10400,94,0x00, 0, 2},
//        {10560,96,0x67, 80, 1}, {10640,96,0x00, 0 , 1}, {10800,93,0x6d, 60, 2}, {10860,93,0x00, 0 ,2},
//        {11040,91,0x79, 80, 5}, {11120,91,0x00, 0, 5}, {12000,86,0x76, 80, 6}, {12080,86,0x00, 0, 6},
//        {12480,86,0x73, 72, 7}, {12552,86,0x00, 0, 7}, {13440,84,0x6d, 72, 15}, {13440,88,0x73, 0, 1}, //
//        {13508,88,0x00, 0 ,1}, {13512,84,0x00, 0, 8}, {13920,86,0x76, 84, 2}, {14004,86,0x00, 0, 2},
//        {14400,86,0x76, 72, 3}, {14472,86,0x00, 0, 3}, {15152,81,0x3b, 36, 4}, {15184,83,0x44, 44, 8},
//        {15188,81,0x00, 0, 4}, {15220,84,0x46, 44, 7}, {15228,83,0x00, 0, 8}, {15248,86,0x57, 44, 6},
//        {15264,84,0x00, 0, 7}, {15284,88,0x5c, 36, 5}, {15292,86,0x00, 0, 6}, {15308,89,0x68, 36, 8},
//        {15320,88,0x00, 0, 5}, {15336,91,0x6d, 32,7}, {15344,89,0x00, 0, 8}, {15364,93,0x6d, 96, 6},
//        {15368,91,0x00, 0, 7}, {15460,93,0x00, 0, 6}

        //lean on me
            {480,60,0x75,1,2}, {1200,60,0x00,0,2}, {1300,60,0x76,1,2}, {1700,60,0x00,0,2},
            {1800,62,0x75,1,3}, {2200,62,0x00,0,3}, {2300,64,0x75,1,4}, {2700,64,0x00,0,4},
            {2800,65,0x75,1,5}, {3200,65,0x00,0,5}, {4000,65,0x75,1,5},{4400,65,0x00,0,5},
            {4500,64,0x75,1,4}, {4900,64,0x00,0,4}, {5000,62,0x75,1,3}, {5400,62,0x00,0,3},
            {5500,60,0x75,1,2}, {5900,60,0x00,0,2},{6700,60,0x75,1,2},{7100,60,0x00,0,2},
            {7200,62,0x75,1,3},{7600,62,0x0,0,3},{7700,64,0x75,1,4},{8100,64,0x0,0,4},
            {8200,64,0x75,1,4},{8600,64,0x0,0,4},{8700,62,0x75,1,3},{9100,62,0x00,0,3},
            {9200,62,0x75,1,3},{9600,62,0x00,0,3}, {10400,60,0x75,1,2}, {11200,60,0x00,0,2},
            {11300,60,0x75,1,2},{11700,60,0x00,0,2},{11800,62,0x75,1,3},{12200,62,0x00,0,3},
            {12300,64,0x75,1,4}, {12700,64,0x00,0,4}, {12800,65,0x75,1,5}, {13200,65,0x00,0,5},
            {14000,65,0x75,1,5},{14400,65,0x00,0,5}, {14500,64,0x75,1,4}, {14900,64,0x00,0,4},
            {15000,62,0x75,1,3}, {15400,62,0x00,0,3}, {15500,60,0x75,1,2}, {15900,60,0x00,0,2},
            {16700,60,0x75,1,2},{17100,60,0x00,0,2},{17200,62,0x75,1,3},{17600,62,0x0,0,3},
            {17700,64,0x75,1,4},{18100,64,0x0,0,4},{18200,59,0x75,1,1},{18800,59,0x00,0,1},
            {18900,60,0x75,1,2},{19300,60,0x0,0,2},{19400,60,0x75,1,2},{19800,60,0x0,0,2}

};


int time = 0;
int n = 0;
int j = 0;
int count = 0;
float score;
int8_t flag = 0;
void TIM2_IRQHandler(void)
{
    TIM2->SR = ~TIM_SR_UIF;

    while(events[n].when == (time-1800)) {
        // If the volume is 0, that means turn the note off.
        note_on(0,0,events[n].note, events[n].volume);
        flag = 1;
        n++;
    }
    // Look at the next item in the event array and check if it is
    // time to play that note.
    for (int i=0; i < 78; i++){
        if (events[i].when == time && events[i].duration != 0){
            set_flag(events[i].track-1);
        }
    }

    // Increment the time by one tick.
    time += 1;

    //When we reach the end of the event array, start over.
    if ( n >= sizeof events / sizeof events[0]) {
        spi1_display1("Game is over");
        spi1_display2("bitch");
    }

    //spi1_display1("Game is over bitch");
}


// Configure timer 2 so that it invokes the Update interrupt
// every n microseconds.  To do so, set the prescaler to divide
// by 48.  Then the CNT will count microseconds up to the ARR value.
// Basically ARR = n-1
// Call NVIC_SetPriority() to set a low priority for Timer 2 interrupt.
// See the lab 6 text to understand how to do so.
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
    TIM7->PSC = 25;
    TIM7->ARR = 480000-1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<18;
    NVIC_SetPriority(TIM7_IRQn,3);
}

void init_tim15(void){
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 0;
    TIM15->ARR = 4800-1;
    TIM15->DIER |= TIM_DIER_UIE;
    TIM15->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<20;
    NVIC_SetPriority(TIM15_IRQn,3);
}

void TIM7_IRQHandler(void){
    TIM7->SR &= ~(1<<0);
    for (int i=0;i<16;i++){
        move_tiles(i);
    }
}

void TIM15_IRQHandler(void){
    TIM15->SR &= ~(1<<0);
    int a = (GPIOA->IDR) & 1;
    if (a == 1){
        init_tim7();
    }
}

void init_tim3(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 0;
    TIM3->ARR = 4800-1;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<16;
}

void TIM3_IRQHandler(void){
    TIM3->SR &= ~(1<<0);

    int a = (GPIOC->IDR >> 7) & 1;

    if (a == 1){
        spi1_display1("Score:           ");
        spi1_display2("             ");
        TIM3->DIER &= ~TIM_DIER_UIE;
        init_tim14();
        init_tim15();
        init_tim7();
        init_wavetable_hybrid2();      // set up wavetable form
        init_dac();         // initialize the DAC
        init_tim6();        // initialize TIM6
        init_tim2(1000); // initialize TIM2
    }
}

void TIM14_IRQHandler(void){

    TIM14->SR &= ~(1<<0);
    int arr[8];
    char score_string[20];

    arr[0] = GPIOC->IDR & 0x80; //PC7
    arr[1] = GPIOC->IDR & 0x100; //PC8
    arr[2] = GPIOC->IDR & 0x200; //PC9
    arr[3] = GPIOA->IDR & 0x100; //PA8
    arr[4] = GPIOA->IDR & 0x200; //PA9
    arr[5] = GPIOA->IDR & 0x400; //PA10
    arr[6] = GPIOA->IDR & 0x800; //PA11
    arr[7] = GPIOA->IDR & 0x1000; //PA12
    for(int i = 0; i < 16; i++) {
        if(arr[i%8] != 0){
            if(check_range(i)) {
                score+= 2;
            }
            else{
                score--;
            }

            sprintf(score_string,"%5.0f", score);
            spi1_display1("Score:");
            spi1_display2(score_string);
        }
    }
}

void init_tim14(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->PSC = 0;
    TIM14->ARR = 480000-1;
    TIM14->DIER |= TIM_DIER_UIE;
    TIM14->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<19;
    NVIC_SetPriority(TIM14_IRQn,3);
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

//void basic_drawing(void);
//void move_ball(void);

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

    LCD_Clear(0);
    LCD_DrawFillRectangle(0, 290, 240, 290, WHITE);
    LCD_DrawFillRectangle(0, 318, 240, 318, WHITE);

    LCD_DrawFillRectangle(0, 0, 0, 320, WHITE);
    LCD_DrawFillRectangle(30, 0, 30, 320, WHITE);
    LCD_DrawFillRectangle(60, 0, 60, 320, WHITE);
    LCD_DrawFillRectangle(90, 0, 90, 320, WHITE);
    LCD_DrawFillRectangle(120, 0, 120, 320, WHITE);
    LCD_DrawFillRectangle(150, 0, 150, 320, WHITE);
    LCD_DrawFillRectangle(180, 0, 180, 320, WHITE);
    LCD_DrawFillRectangle(210, 0, 210, 320, WHITE);
    LCD_DrawFillRectangle(240, 0, 240, 320, WHITE);

    init_tim3();

    init_spi1();
    spi1_init_oled();

    spi1_display1("Press any button");
    spi1_display2("to start");

}

