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
    int color;
} tiles[] = {{1,-20,29,0,0,RED},
        {31,-20,59,0,0,YELLOW},
        {61,-20,89,0,0,BLUE},
        {91,-20,119,0,0,GREEN},
        {121,-20,149,0,0,CYAN},
        {151,-20,179,0,0,MAGENTA},
        {181,-20,209,0,0,BRRED},
        {211,-20,239,0,0,BROWN},
        {1,-20,29,0,0,RED},
        {31,-20,59,0,0,YELLOW},
        {61,-20,89,0,0,BLUE},
        {91,-20,119,0,0,GREEN},
        {121,-20,149,0,0,CYAN},
        {151,-20,179,0,0,MAGENTA},
        {181,-20,209,0,0,BRRED},
        {211,-20,239,0,0,BROWN},
};



void set_flag(int n){
    tiles[n].flag = 1;

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
           LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y1, tiles[tile_num].x2, tiles[tile_num].y1+1, BLACK);

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
        LCD_DrawFillRectangle(tiles[tile_num].x1, 290, tiles[tile_num].x2, 290, WHITE);
    }

    if (tiles[tile_num].y2 >= 318){
        LCD_DrawFillRectangle(tiles[tile_num].x1, 318, tiles[tile_num].x2, 318, WHITE);
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
        tiles[tile_num].y1 -= 340;
        tiles[tile_num].y2 -= 340;
        clear_flag(tile_num);
    }
}

//chekc tiles are in the range
int check_range(int n){
    if(tiles[n].y1 > 270 && tiles[n].y1 < 320){
        tiles[n].color = BLACK;
        return 1;}
    else
        return 0;
}
