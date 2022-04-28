#include "stm32f0xx.h"
#include "lcd.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "midiplay.h"
#include <math.h>
#include <unistd.h>
#include <stdint.h>

void move_tiles(int tile_num);
void spi1_display2(const char *string);
void spi1_display1(const char *string);
void spi1_init_oled();
void init_spi1();
//void init_lcd_spi(void);
void setup_buttons(void);
void internal_clock();
void init_dac(void);

void init_tim2(int n);
void init_tim3(void);
void init_tim6(void);
void init_tim7(void);
void init_tim14(void);
void init_tim15(void);

int check_range(int n);
void move_tiles(int tile_num);

void set_flag(int n);
void set_next(int tile_num, int duration);

float score = 0;
char score_string[10];

int a; //first button input
int b; //second button input
int a2;
int b2;

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

// An array of structures that say when to turn notes on or off.
struct {
    uint16_t when;
    uint16_t note;
    uint16_t volume;
    uint16_t duration;
    uint16_t track;

} events[] = {
        {480,84,0x75,76, 1}, {556,84,0x00,0, 1}, {960,84,0x75,48, 2}, {1008,84,0x00,0, 2},
        {1440,91,0x75,80, 3}, {1520,91,0x00,0, 3}, {1920,91,0x75,76, 4}, {1996,91,0x00,0, 4},
        {2400,93,0x76,72, 5}, {2472,93,0x00,0, 5}, {2640,94,0x67,80, 6}, {2720,94,0x00,0, 6},
        {2880,96,0x67,80, 7}, {2960,96,0x00,0, 7}, {3120,93,0x6d,60, 8}, {3180,93,0x00,0, 8},
        {3360,91,0x79,80, 9}, {3440,91,0x00,0, 7}, {4320,89,0x70,88, 10}, {4408,89,0x00,0, 6},
        {4800,89,0x73, 84, 11}, {4884,89,0x00, 0, 5}, {5280,88,0x73, 80, 12}, {5360,88,0x00, 0, 4},
        {5760,91,0x79, 76, 13}, {5836,91,0x00, 0, 3}, {6240,86,0x79, 68, 14}, {6308,86,0x00, 0, 2},
        {6720,86,0x76, 48, 1}, {6768,86,0x00, 0, 1}, {7200,84,0x76, 52, 7}, {7252,84,0x00, 0, 7},
        {8160,84,0x73, 76, 6}, {8236,84,0x00,0, 6}, {8640,84,0x74, 48, 5}, {8688,84,0x00, 0, 5},
        {9120,91,0x76, 80, 4}, {9200,91,0x00, 0, 4}, {9600,91,0x79, 76, 3}, {9676,91,0x00, 0 , 3},
        {10080,93,0x76, 72, 1}, {10152,93,0x00, 0,1}, {10320,94,0x67, 80, 2}, {10400,94,0x00, 0, 2},
        {10560,96,0x67, 80, 9}, {10640,96,0x00, 0 , 1}, {10800,93,0x6d, 60, 10}, {10860,93,0x00, 0 ,2},
        {11040,91,0x79, 80, 5}, {11120,91,0x00, 0, 5}, {12000,86,0x76, 80, 6}, {12080,86,0x00, 0, 6},
        {12480,86,0x73, 72, 7}, {12552,86,0x00, 0, 7}, {13440,84,0x6d, 72, 15}, {13440,88,0x73, 0, 1}, //
        {13508,88,0x00, 0 ,1}, {13512,84,0x00, 0, 8}, {13920,86,0x76, 74, 2}, {14004,86,0x00, 0, 2},
        {14400,86,0x76, 72, 3}, {14472,86,0x00, 0, 3}, {15152,81,0x3b, 36, 4}, {15184,83,0x44, 44, 8},
        {15188,81,0x00, 0, 4}, {15220,84,0x46, 44, 7}, {15228,83,0x00, 0, 8}, {15248,86,0x57, 44, 6},
        {15264,84,0x00, 0, 7}, {15284,88,0x5c, 36, 5}, {15292,86,0x00, 0, 6}, {15308,89,0x68, 36, 8},
        {15320,88,0x00, 0, 5}, {15336,91,0x6d, 32,7}, {15344,89,0x00, 0, 8}, {15364,93,0x6d, 96, 6},
        {15368,91,0x00, 0, 7}, {15460,93,0x00, 0, 6}, {16000, 93, 0x00, 0, 6}



};

struct {
    uint16_t when;
    uint16_t note;
    uint16_t volume;
    uint16_t duration;
    uint16_t track;

} events2[] = {
        {480,60,0x75,1,2}, {1200,60,0x00,0,2}, {1300,60,0x76,1,10}, {1700,60,0x00,0,2},
        {1800,62,0x75,1,3}, {2200,62,0x00,0,3}, {2300,64,0x75,1,4}, {2700,64,0x00,0,4},
        {2800,65,0x75,1,5}, {3200,65,0x00,0,5}, {4000,65,0x75,1,13},{4400,65,0x00,0,5},
        {4500,64,0x75,1,4}, {4900,64,0x00,0,4}, {5000,62,0x75,1,3}, {5400,62,0x00,0,3},
        {5500,60,0x75,1,2}, {5900,60,0x00,0,2},{6700,60,0x75,1,10},{7100,60,0x00,0,2},
        {7200,62,0x75,1,3},{7600,62,0x0,0,3},{7700,64,0x75,1,4},{8100,64,0x0,0,4},
        {8200,64,0x75,1,12},{8600,64,0x0,0,4},{8700,62,0x75,1,11},{9100,62,0x00,0,3},
        {9200,62,0x75,1,3},{9600,62,0x00,0,3}, {10400,60,0x75,1,2}, {11200,60,0x00,0,2},
        {11300,60,0x75,1,10},{11700,60,0x00,0,2},{11800,62,0x75,1,3},{12200,62,0x00,0,3},
        {12300,64,0x75,1,4}, {12700,64,0x00,0,4}, {12800,65,0x75,1,5}, {13200,65,0x00,0,5},
        {14000,65,0x75,1,13},{14400,65,0x00,0,5}, {14500,64,0x75,1,4}, {14900,64,0x00,0,4},
        {15000,62,0x75,1,3}, {15400,62,0x00,0,3}, {15500,60,0x75,1,2}, {15900,60,0x00,0,2},
        {16700,60,0x75,1,10},{17100,60,0x00,0,2},{17200,62,0x75,1,3},{17600,62,0x0,0,3},
        {17700,64,0x75,1,4},{18100,64,0x0,0,4},{18200,59,0x75,1,1},{18800,59,0x00,0,1},
        {18900,60,0x75,1,2},{19300,60,0x0,0,2},{19400,60,0x75,1,10},{19800,60,0x0,0,2}
};


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


int time = 0;
int n = 0;
float score;
void TIM2_IRQHandler(void)
{
    TIM2->SR = ~TIM_SR_UIF;
    if (a2 == 1){
        while(events[n].when == (time-1800)) {
        // If the volume is 0, that means turn the note off.
            note_on(0,0,events[n].note, events[n].volume);
            n++;
        }
        for (int i=0; i < sizeof events / sizeof events[0]; i++){
            if (events[i].when == time && events[i].duration != 0){
                set_flag(events[i].track-1);
            //216-227
            if(events[i+1].duration != 0){
                set_next(events[i+1].track-1, events[i+1].duration);
            }
            else if(events[i+2].duration != 0){
                set_next(events[i+2].track-1, events[i+2].duration);
            }
            else if(events[i+3].duration != 0){
                set_next(events[i+3].track-1, events[i+3].duration);
            }
            else if(events[i+4].duration != 0){
                set_next(events[i+4].track-1, events[i+4].duration);
            }
            }
        }

    // Increment the time by one tick.
        time += 1;

    //When we reach the end of the event array, start over.
        if(n >= sizeof events/sizeof events[0]) {

            LCD_DrawPicture(0, 0, &background);


            int first_pressed = (GPIOC->IDR >> 7) & 1;
            if (first_pressed)
                NVIC_SystemReset(); // go back to the main menu
        }
    }
    else if (b2 == 1){
        while(events2[n].when == (time-1600)) {
            note_on(0,0,events2[n].note, events2[n].volume);
            n++;
        }

        for (int i=0; i < sizeof events2 / sizeof events2[0]; i++){
            if (events2[i].when == time && events2[i].duration != 0){
                set_flag(events2[i].track-1);
            }
            if(events2[i+1].duration != 0){
                set_next(events2[i+1].track-1, events2[i+1].duration);
            }
            else if(events2[i+2].duration != 0){
                set_next(events2[i+2].track-1, events2[i+2].duration);
            }
            else if(events2[i+3].duration != 0){
                set_next(events2[i+3].track-1, events2[i+3].duration);
            }
            else if(events2[i+4].duration != 0){
                set_next(events2[i+4].track-1, events2[i+4].duration);
            }
         }

    // Increment the time by one tick.
        time += 1;

    //When we reach the end of the event array, start over.
        if ( n >= sizeof events2 / sizeof events2[0]) {

            int first_pressed = (GPIOC->IDR >> 7) & 1;
            if (first_pressed)
                NVIC_SystemReset();

        }

    }
}

void TIM3_IRQHandler(void){
    TIM3->SR &= ~(1<<0);

    a = (GPIOC->IDR >> 7) & 1;
    b = (GPIOC->IDR >> 8) & 1;
    if (a == 1 || b == 1){
        LCD_Clear(0xFFFF);
        LCD_DrawFillRectangle(0, 290, 240, 290, 0);
        LCD_DrawFillRectangle(0, 318, 240, 318, 0);

        LCD_DrawFillRectangle(0, 0, 0, 320, 0);
        LCD_DrawFillRectangle(30, 0, 30, 320, 0);
        LCD_DrawFillRectangle(60, 0, 60, 320, 0);
        LCD_DrawFillRectangle(90, 0, 90, 320, 0);
        LCD_DrawFillRectangle(120, 0, 120, 320, 0);
        LCD_DrawFillRectangle(150, 0, 150, 320, 0);
        LCD_DrawFillRectangle(180, 0, 180, 320, 0);
        LCD_DrawFillRectangle(210, 0, 210, 320, 0);
        LCD_DrawFillRectangle(240, 0, 240, 320, 0);
        spi1_display1("Score:           ");
        spi1_display2("             ");
        a2 = a;
        b2 = b;
        TIM3->DIER &= ~TIM_DIER_UIE;
        TIM3->CR1 &= ~TIM_CR1_CEN;
        init_tim14();
        init_tim15();
        init_tim7();
        init_wavetable_hybrid2();      // set up wavetable form
        init_dac();         // initialize the DAC
        init_tim6();        // initialize TIM6
        init_tim2(1000); // initialize TIM2
    }
}

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

// We'll use the Timer 6 IRQ to recompute samples and feed those
// samples into the DAC.
void TIM6_DAC_IRQHandler(void)
{
    TIM6->SR = ~TIM_SR_UIF;


    int sample = 0;
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

void TIM7_IRQHandler(void){
    TIM7->SR &= ~(1<<0);
    for (int i=0;i<16;i++){
        move_tiles(i);
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
        if(check_range(i)) {
            if(arr[i%8] != 0){
                change_color(i);
                score+= 2;
            }
            else{
                score -=0.1;
                // Have another check range, then note_off()
//                if(tiles[n].y1 == 320){
//                    note_off(0, 0, events[n].note, events[n]);
//                }
            }

        }
        else{
            if(arr[i%8] != 0){
//                arr[i%8] = 0;
                score -= 0.5;
            }
        }
    }
}

void TIM15_IRQHandler(void){
    TIM15->SR &= ~(1<<0);
    sprintf(score_string,"%5.0f", score);
    spi1_display1("Score:       ");
    spi1_display2("                ");
    spi1_display2(score_string);
}

extern const Picture background;

int main(void)
{
    internal_clock();
    setup_buttons();
    LCD_Setup(); // this will call init_lcd_spi()

    LCD_DrawPicture(0, 0, &background);

//    LCD_Clear(0);
//    LCD_DrawFillRectangle(0, 290, 240, 290, WHITE);
//    LCD_DrawFillRectangle(0, 318, 240, 318, WHITE);
//
//    LCD_DrawFillRectangle(0, 0, 0, 320, WHITE);
//    LCD_DrawFillRectangle(30, 0, 30, 320, WHITE);
//    LCD_DrawFillRectangle(60, 0, 60, 320, WHITE);
//    LCD_DrawFillRectangle(90, 0, 90, 320, WHITE);
//    LCD_DrawFillRectangle(120, 0, 120, 320, WHITE);
//    LCD_DrawFillRectangle(150, 0, 150, 320, WHITE);
//    LCD_DrawFillRectangle(180, 0, 180, 320, WHITE);
//    LCD_DrawFillRectangle(210, 0, 210, 320, WHITE);
//    LCD_DrawFillRectangle(240, 0, 240, 320, WHITE);

    init_tim3();

    init_spi1();
    spi1_init_oled();

    spi1_display1("Press first two");
    spi1_display2("buttons");
}
