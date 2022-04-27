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
} tiles[] = {{1,-20,29,0,0,WHITE},
        {31,-20,59,0,0,WHITE},
        {61,-20,89,0,0,WHITE},
        {91,-20,119,0,0,WHITE},
        {121,-20,149,0,0,WHITE},
        {151,-20,179,0,0,WHITE},
        {181,-20,209,0,0,WHITE},
        {211,-20,239,0,0,WHITE},
        {1,-20,29,0,0,WHITE},
        {31,-20,59,0,0,WHITE},
        {61,-20,89,0,0,WHITE},
        {91,-20,119,0,0,WHITE},
        {121,-20,149,0,0,WHITE},
        {151,-20,179,0,0,WHITE},
        {181,-20,209,0,0,WHITE},
        {211,-20,239,0,0,WHITE},
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
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, RED);
           }
           else if(tiles[tile_num].x1 == 31){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, YELLOW);
           }
           else if(tiles[tile_num].x1 == 61){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, BLUE);
           }
           else if(tiles[tile_num].x1 == 91){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, GREEN);
           }
           else if(tiles[tile_num].x1 == 121){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, CYAN);
           }
           else if(tiles[tile_num].x1 == 151){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, MAGENTA);
           }
           else if(tiles[tile_num].x1 == 181){
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, BRRED);
           }
           else{
               LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2, BROWN);
           }
    tiles[tile_num].y1 += 2;
    tiles[tile_num].y2 += 2;
    }

    if (tiles[tile_num].y1 > 270){
        LCD_DrawFillRectangle(tiles[tile_num].x1, 290, tiles[tile_num].x2, 290, WHITE);
    }

    if (tiles[tile_num].y2 >= 318){
        LCD_DrawFillRectangle(tiles[tile_num].x1, 318, tiles[tile_num].x2, 318, WHITE);
    }


    if (tiles[tile_num].y1 == 320){
        tiles[tile_num].y1 -= 340;
        tiles[tile_num].y2 -= 340;
        clear_flag(tile_num);
    }
}

//chekc tiles are in the range
int check_range(int n){
    if(tiles[n].y1 > 250 && tiles[n].y1 < 320)
        return 1;
    else
        return 0;
}
