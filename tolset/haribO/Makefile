TOOLPATH = ../z_tools/
MAKE = $(TOOLPATH)make.exe -r
NASK = $(TOOLPATH)nask.exe
EDIMG = $(TOOLPATH)edimg.exe
IMGTOL = $(TOOLPATH)imgtol.exe


default:
	$(MAKE) img

# Commands

clean:
	del ipl.lst 
	del ipl.bin
	del haribote.sys
	del haribote.lst

src_only:
	$(MAKE) clean
	del haribote.img

asm: 
	$(MAKE) ipl.bin

img: 
	$(MAKE) haribote.img

run: haribote.img 
	copy haribote.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install: img
	$(IMGTOL) w a: haribote.img

# Files

ipl.bin: ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

haribote.sys: haribote.nas
	$(NASK) haribote.nas haribote.sys haribote.lst

haribote.img: ipl.bin haribote.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
	wbinimg src:ipl.bin len:512 from:0 to:0  \
	copy from:haribote.sys to:@: 			 \
	imgout:haribote.img