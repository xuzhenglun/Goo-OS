goo.img : os.img bootblock.tmp
	rm ./tmp -rf
	mkdir ./tmp
	sudo mount -o loop os.img ./tmp
	sudo cp bootblock.tmp ./tmp/bootblock.sys
	sudo umount ./tmp
	mv os.img goo.img

black.tmp : ./src/app/black.s
	nasm ./src/app/black.s -o black.tmp

IPL.tmp : ./src/boot/IPL.s
	nasm ./src/boot/IPL.s -o IPL.tmp

os.img : IPL.tmp
	cp IPL.tmp os.img
	dd if=/dev/zero of=os.img bs=512 seek=1 count=2879

bootblock.tmp : bootblock.o
	objcopy -j .text -S -O binary bootblock.o bootblock.tmp

bootblock.o : bootasm.o bootmain.o
	ld  -m elf_i386  -e start -Ttext 0xc400 -o bootblock.o bootasm.o bootmain.o

bootasm.o : ./src/boot/bootasm.s
	nasm -f elf ./src/boot/bootasm.s -o bootasm.o

bootmain.o : ./src/boot/bootmain.c
	gcc -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -nostdinc -I. -c ./src/boot/bootmain.c

debug : goo.img
	qemu-system-i386 -boot order=a -fda ./goo.img -m 8 -S -s

run : goo.img
	qemu-system-i386 -boot order=a -fda ./goo.img -m 8

clean : 
	find . -name "*.tmp"  | xargs rm -f
	find . -name "*.img"  | xargs rm -f
	find . -name "*.o"  | xargs rm -f
	find . -name "*.d"  | xargs rm -f
	rm ./tmp -rf
