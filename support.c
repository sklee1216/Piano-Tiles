#include "stm32f0xx.h"
#include <stdint.h>
#include <stdlib.h>
#include "lcd.h"

struct tiles {
    int x1;
    int y1;
    int x2;
    int y2;
};

struct tiles t1;
struct tiles t2;
struct tiles t3;
struct tiles t4;
struct tiles t5;
struct tiles t6;
struct tiles t7;
struct tiles t8;



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

void move_tiles(int tile_num)
{
    if (tile_num == 1){

        LCD_DrawFillRectangle(t1.x1,t1.y2+1,t1.x2,t1.y2+1,WHITE);
        LCD_DrawFillRectangle(t1.x1,t1.y1,t1.x2,t1.y1,BLACK);
        t1.y1 += 1;
        t1.y2 += 1;

       }

       if (tile_num == 2){
           LCD_DrawFillRectangle(t2.x1,t2.y2+1,t2.x2,t2.y2+1,YELLOW);
           LCD_DrawFillRectangle(t2.x1,t2.y1,t2.x2,t2.y1,BLACK);
           t2.y1 += 1;
           t2.y2 += 1;
       }

       if (tile_num == 3){
           LCD_DrawFillRectangle(t3.x1,t3.y2+1,t3.x2,t3.y2+1,MAGENTA);
           LCD_DrawFillRectangle(t3.x1,t3.y1,t3.x2,t3.y1,BLACK);
           t3.y1 += 1;
           t3.y2 += 1;

               }
       if (tile_num == 4){
           LCD_DrawFillRectangle(t4.x1,t4.y2+1,t4.x2,t4.y2+1,WHITE);
           LCD_DrawFillRectangle(t4.x1,t4.y1,t4.x2,t4.y1,BLACK);
           t4.y1 += 1;
           t4.y2 += 1;
               }
       if (tile_num == 5){
           LCD_DrawFillRectangle(t5.x1,t5.y2+1,t5.x2,t5.y2+1,RED);
           LCD_DrawFillRectangle(t5.x1,t5.y1,t5.x2,t5.y1,BLACK);
           t5.y1 += 1;
           t5.y2 += 1;
               }
       if (tile_num == 6){
           LCD_DrawFillRectangle(t6.x1,t6.y2+1,t6.x2,t6.y2+1,CYAN);
           LCD_DrawFillRectangle(t6.x1,t6.y1,t6.x2,t6.y1,BLACK);
           t6.y1 += 1;
           t6.y2 += 1;
               }
       if (tile_num == 7){

           LCD_DrawFillRectangle(t7.x1,t7.y2+1,t7.x2,t7.y2+1,BROWN);
           LCD_DrawFillRectangle(t7.x1,t7.y1,t7.x2,t7.y1,BLACK);
           t7.y1 += 1;
           t7.y2 += 1;
               }
       if (tile_num == 8){
           LCD_DrawFillRectangle(t8.x1,t8.y2+1,t8.x2,t8.y2+1,GREEN);
           LCD_DrawFillRectangle(t8.x1,t8.y1,t8.x2,t8.y1,BLACK);
           t8.y1 += 1;
           t8.y2 += 1;
               }
}






void init_tiles(void){
    LCD_Clear(0);

    t1.x1 = 0;
    t2.x1 = 30;
    t3.x1 = 60;
    t4.x1 = 90;
    t5.x1 = 120;
    t6.x1 = 150;
    t7.x1 = 180;
    t8.x1 = 210;

    t1.x2 = 30;
    t2.x2 = 60;
    t3.x2 = 90;
    t4.x2 = 120;
    t5.x2 = 150;
    t6.x2 = 180;
    t7.x2 = 210;
    t8.x2 = 240;

    t1.y1 = 0;
    t2.y1 = 0;
    t3.y1 = 0;
    t4.y1 = 0;
    t5.y1 = 0;
    t6.y1 = 0;
    t7.y1 = 0;
    t8.y1 = 0;

    t1.y2 = 20;
    t2.y2 = 20;
    t3.y2 = 20;
    t4.y2 = 20;
    t5.y2 = 20;
    t6.y2 = 20;
    t7.y2 = 20;
    t8.y2 = 20;

    LCD_DrawFillRectangle(t1.x1, t1.y1, t1.x2, t1.y2, GRAY);
    LCD_DrawFillRectangle(t2.x1, t2.y1, t2.x2, t2.y2, YELLOW);
    LCD_DrawFillRectangle(t3.x1, t3.y1, t3.x2, t3.y2, MAGENTA);
    LCD_DrawFillRectangle(t4.x1, t4.y1, t4.x2, t4.y2, WHITE);
    LCD_DrawFillRectangle(t5.x1, t5.y1, t5.x2, t5.y2, RED);
    LCD_DrawFillRectangle(t6.x1, t6.y1, t6.x2, t6.y2, CYAN);
    LCD_DrawFillRectangle(t7.x1, t7.y1, t7.x2, t7.y2, BROWN);
    LCD_DrawFillRectangle(t8.x1, t8.y1, t8.x2, t8.y2, GREEN);
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

