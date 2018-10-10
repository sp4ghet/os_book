#include <stdio.h>
#include "bootpack.h"
#include "graphics.h"
#include "dsctable.h"

struct BOOTINFO {
    char cylinders, leds, videoMode, reserve;
    short screenX, screenY;
    char* vram;
};


void HariMain(void){
    
    init_palette();

    struct BOOTINFO *binfo = (struct BOOTINFO*) 0x00ff0;

    int xsize = binfo->screenX;
    int ysize = binfo->screenY;
    char* vram = binfo->vram;
    
    init_gdtidt();
    init_screen(vram, xsize, ysize);

    // putfont8(vram, xsize, 8, 8, COL8_ffffff, hankaku + 'A'*16);
    putfonts8_asci(vram, xsize, 8, 8, COL8_ffffff, "ABC 123");
    putfonts8_asci(vram, xsize, 31, 31, COL8_000000, "Haribote os.");
    putfonts8_asci(vram, xsize, 30, 30, COL8_ffffff, "Haribote os.");

    char* s = "";
    sprintf(s, "cylinders = 0x%05x", binfo->cylinders);
    putfonts8_asci(vram, xsize, 30, 60, COL8_ffffff, s);

    char* mcursor = "";
    int mx = 160,
        my = 100;
    init_cursor(mcursor, COL8_008484);
    putblock8_8(vram, xsize, 16, 16, mx, my, mcursor, 16);

    for(;;){
        io_hlt();
    }

}
