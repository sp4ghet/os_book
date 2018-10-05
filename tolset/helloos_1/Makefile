default: img

# Commands

clean:
	del ipl.lst ipl.bin

src_only: clean
	del helloos.img

asm: ipl.bin

img: asm helloos.img

run: img
	copy helloos.img ..\z_tools\qemu\fdimage0.bin
	..\z_tools\make.exe -C ../z_tools/qemu

install: img
	..\z_tools\imgtol.com w a: helloos.img

# Files

ipl.bin: ipl.nas Makefile
	..\z_tools\nask.exe ipl.nas ipl.bin ipl.lst

helloos.img: ipl.bin Makefile
	..\z_tools\edimg.exe   imgin:../z_tools/fdimg0at.tek \
	wbinimg src:ipl.bin len:512 from:0 to:0   imgout:helloos.img