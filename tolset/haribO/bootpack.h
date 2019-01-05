#ifndef BOOTPACK_H
#define BOOTPACK_H

#define ADR_BOOTINFO 0x00ff0

struct BOOTINFO {
    char cylinders, leds, videoMode, reserve;
    short screenX, screenY;
    char* vram;
};

// functions in bootpack.c
void HariMain(void);
void make_window8(unsigned char *buf, int xsize, int ysize, char *title);

#endif
