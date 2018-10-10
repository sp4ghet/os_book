#ifndef GRAPHICS_H
#define GRAPHICS_H

#define COL8_000000 0
#define COL8_ff0000 1
#define COL8_00ff00 2
#define COL8_ffff00 3
#define COL8_0000ff 4
#define COL8_ff00ff 5
#define COL8_00ffff 6
#define COL8_ffffff 7
#define COL8_c6c6c6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char* vram, int screenX, int screenY);

void putfont8(char* vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asci(char* vram, int xsize, int x, int y, char color, unsigned char *s);

void init_cursor(char *mouse, char color);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);

#endif
