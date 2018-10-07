void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

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

void HariMain(void){
    
    init_palette();

    char* vram = (char*) 0xa0000;

    int xsize = 320;
    int ysize = 200;
    
    boxfill(vram, xsize, COL8_008484, 0,          0, xsize - 1, ysize - 29);
    boxfill(vram, xsize, COL8_c6c6c6, 0, ysize - 28, xsize - 1, ysize - 28);
    boxfill(vram, xsize, COL8_ffffff, 0, ysize - 27, xsize - 1, ysize - 27);
    boxfill(vram, xsize, COL8_c6c6c6, 0, ysize - 26, xsize - 1, ysize -  1);

    boxfill(vram, xsize, COL8_ffffff,  3, ysize - 24, 59, ysize - 24);
    boxfill(vram, xsize, COL8_ffffff,  2, ysize - 24,  2, ysize - 4);
    boxfill(vram, xsize, COL8_848484,  3, ysize -  4, 59, ysize - 4);
    boxfill(vram, xsize, COL8_848484, 59, ysize - 23, 59, ysize - 5);
    boxfill(vram, xsize, COL8_000000,  2, ysize -  3, 59, ysize - 3);
    boxfill(vram, xsize, COL8_000000, 60, ysize - 24, 60, ysize - 3);


    boxfill(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
    boxfill(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
    boxfill(vram, xsize, COL8_000000, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
    boxfill(vram, xsize, COL8_000000, xsize -  3, ysize - 24, xsize -  3, ysize -  3);

    for(;;){
        io_hlt();
    }

}


void boxfill(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
    
    int x,y;

    vram += y0*xsize;
    
    for(y=y0; y <= y1; y++){
        for(x=x0; x <= x1; x++){
            vram[x] = c;
        }
        vram += xsize;
    }
}

void init_palette(void){
    static unsigned char table_rgb[16*3] = {
        0x00, 0x00, 0x00, // 0
        0xff, 0x00, 0x00,
        0x00, 0xff, 0x00,
        0xff, 0xff, 0x00,
        0x00, 0x00, 0xff, // 4
        0xff, 0x00, 0xff, 
        0x00, 0xff, 0xff,
        0xff, 0xff, 0xff,
        0xc6, 0xc6, 0xc6,
        0x84, 0x00, 0x00, // 9
        0x00, 0x84, 0x00, 
        0x84, 0x84, 0x00, 
        0x00, 0x00, 0x84,
        0x84, 0x00, 0x84,
        0x00, 0x84, 0x84, // 14
        0x84, 0x84, 0x84 
    };

    set_palette(0, 15, table_rgb);
    return;
}

void set_palette(int start, int end, unsigned char *rgb){
    int i, eflags;
    eflags = io_load_eflags();
    io_cli();
    io_out8(0x03c8, start);

    for(i=start; i <= end; i++ ){
        io_out8(0x03c9, rgb[0] /4);
        io_out8(0x03c9, rgb[1] /4);
        io_out8(0x03c9, rgb[2] /4);
        rgb += 3;
    }
    io_store_eflags(eflags);
    return;
}
