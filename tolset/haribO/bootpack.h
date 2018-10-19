#ifndef BOOTPACK_H
#define BOOTPACK_H

#define ADR_BOOTINFO 0x00ff0

// functions defined in naskfunc.nas
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

struct BOOTINFO {
    char cylinders, leds, videoMode, reserve;
    short screenX, screenY;
    char* vram;
};

#endif
