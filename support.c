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
} tiles[] = {{0,0,30,0,0},
        {30,-20,60,0,0},
        {60,-20,90,0,0},
        {90,-20,120,0,0},
        {120,-20,150,0,0},
        {150,-20,180,0,0},
        {180,-20,210,0,0},
        {210,-20,240,0,0}};



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


// Copy a subset of a large source picture into a smaller destination.
// sx,sy are the offset into the source picture.
void pic_subset(Picture *dst, const Picture *src, int sx, int sy)
{
    int dw = dst->width;
    int dh = dst->height;
    for(int y=0; y<dh; y++) {
        if (y+sy < 0)
            continue;
        if (y+sy >= src->height)
            break;
        for(int x=0; x<dw; x++) {
            if (x+sx < 0)
                continue;
            if (x+sx >= src->width)
                break;
            dst->pix2[dw * y + x] = src->pix2[src->width * (y+sy) + x + sx];
        }
    }
}


// Overlay a picture onto a destination picture.
// xoffset,yoffset are the offset into the destination picture that the
// source picture is placed.
// Any pixel in the source that is the 'transparent' color will not be
// copied.  This defines a color in the source that can be used as a
// transparent color.
void pic_overlay(Picture *dst, int xoffset, int yoffset, const Picture *src, int transparent)
{
    for(int y=0; y<src->height; y++) {
        int dy = y+yoffset;
        if (dy < 0)
            continue;
        if (dy >= dst->height)
            break;
        for(int x=0; x<src->width; x++) {
            int dx = x+xoffset;
            if (dx < 0)
                continue;
            if (dx >= dst->width)
                break;
            unsigned short int p = src->pix2[y*src->width + x];
            if (p != transparent)
                dst->pix2[dy*dst->width + dx] = p;
        }
    }
}

extern const Picture background; // A 240x320 background image
extern const Picture ball; // A 19x19 purple ball with white boundaries

// This C macro will create an array of Picture elements.
// Really, you'll just use it as a pointer to a single Picture
// element with an internal pix2[] array large enough to hold
// an image of the specified size.
// BE CAREFUL HOW LARGE OF A PICTURE YOU TRY TO CREATE:
// A 100x100 picture uses 20000 bytes.  You have 32768 bytes of SRAM.
#define TempPicturePtr(name,width,height) Picture name[(width)*(height)/6+2] = { {width,height,2} }

void erase(int x, int y)
{
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    //pic_overlay(tmp, 5,5, &ball, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void update2(int x, int y)
{
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, 5,5, &ball, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void move_tiles(int tile_num){

    if (tiles[tile_num].flag == 1){

    LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y2 +1, tiles[tile_num].x2, tiles[tile_num].y2 + 2,WHITE);
    LCD_DrawFillRectangle(tiles[tile_num].x1, tiles[tile_num].y1, tiles[tile_num].x2, tiles[tile_num].y1+1, BLACK);
    tiles[tile_num].y1 += 2;
    tiles[tile_num].y2 += 2;
    }



}






void init_tiles(void){
    LCD_Clear(0);



    LCD_DrawFillRectangle(tiles[0].x1, tiles[0].y1, tiles[0].x2, tiles[0].y2, WHITE);
    LCD_DrawFillRectangle(tiles[1].x1, tiles[1].y1, tiles[1].x2, tiles[1].y2, WHITE);
    LCD_DrawFillRectangle(tiles[2].x1, tiles[2].y1, tiles[2].x2, tiles[2].y2, WHITE);
    LCD_DrawFillRectangle(tiles[3].x1, tiles[3].y1, tiles[3].x2, tiles[3].y2, WHITE);
    LCD_DrawFillRectangle(tiles[4].x1, tiles[4].y1, tiles[4].x2, tiles[4].y2, WHITE);
    LCD_DrawFillRectangle(tiles[5].x1, tiles[5].y1, tiles[5].x2, tiles[5].y2, WHITE);
    LCD_DrawFillRectangle(tiles[6].x1, tiles[6].y1, tiles[6].x2, tiles[6].y2, WHITE);
    LCD_DrawFillRectangle(tiles[7].x1, tiles[7].y1, tiles[7].x2, tiles[7].y2, WHITE);
}


/*void basic_drawing(void)
{
    LCD_Clear(0);
    for(;;)
        LCD_DrawPicture(0,0,&load1);
        LCD_DrawPicture(0,0,&load2);
        if(start){
            break;
        }*/
    /*LCD_Circle(50, 90, 40, 1, CYAN);
    LCD_DrawTriangle(90,10, 120,10, 90,30, YELLOW);
    LCD_DrawFillTriangle(90,90, 120,120, 90,120, GRAY);
    LCD_DrawFillRectangle(10, 140, 120, 159, WHITE);
    LCD_DrawString(20,141, BLACK, WHITE, "Test string!", 16, 0); // opaque background
}*/

