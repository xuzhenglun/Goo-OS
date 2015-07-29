OBJS_BOOTPACK = bootasm.o bootmain.o basic.o font.o sprintf.o vsprintf.o strtoul0.o strlen.o dsctbl.o graph.o int.o int_asm.o fifo.o

INCPATH  = ./../

MAKE     = make
NASM     = nasm
CC       = gcc -O0 -I$(INCPATH) -c -std=c99 -masm=intel -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -funsigned-char -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -c
OBJCOPY  = objcopy


goo.img : os.img bootblock.bin
	rm ./tmp -rf
	mkdir ./tmp
	sudo mount -o loop os.img ./tmp
	sudo cp bootblock.bin ./tmp/bootblock.sys
	sudo umount ./tmp
	mv os.img goo.img

black.bin : ./src/app/black.s
	$(NASK) ./src/app/black.s -o black.bin

IPL.bin : ./src/boot/IPL.s
	$(NASM) ./src/boot/IPL.s -o IPL.bin
	
bootasm.o : ./src/boot/bootasm.s
	$(NASM) -f elf32 ./src/boot/bootasm.s -o bootasm.o

int_asm.o : ./src/boot/int_asm.s
	$(NASM) -f elf32 ./src/boot/int_asm.s -o int_asm.o

#io_out8.o : ./src/boot/io_out8.s
	#$(NASM) -f elf32 ./src/boot/io_out8.s -o io_out8.o

os.img : IPL.bin
	cp IPL.bin os.img
	dd if=/dev/zero of=os.img bs=512 seek=1 count=2879

bootblock.bin : bootblock.o
	$(OBJCOPY)  -O binary bootblock.o bootblock.bin

bootblock.o : $(OBJS_BOOTPACK)
	ld  -m elf_i386 -e start -Ttext 0xc400 -o bootblock.o $(OBJS_BOOTPACK)

makefont.a : ./tools/makefont.c
	gcc -m32  ./tools/makefont.c -o makefont.a

font.o : font.bin
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 font.bin font.o

font.bin : makefont.a ./src/boot/hankaku.txt
	./makefont.a ./src/boot/hankaku.txt font.bin

%.o : ./src/golibc/%.c
	$(CC) ./src/golibc/$*.c
	
%.o : ./src/boot/%.c
	$(CC) ./src/boot/$*.c

debug : goo.img
	qemu-system-i386 -boot order=a -fda ./goo.img -m 8 -S -s

run : goo.img
	qemu-system-i386 -boot order=a -fda ./goo.img -m 32

clean :
	#sudo umount ./tmp
	find . -name "*.img"  | xargs rm -f
	find . -name "*.o"  | xargs rm -f
	find . -name "*.d"  | xargs rm -f
	find . -name "*.a"  | xargs rm -f
	find . -name "*.bin"  | xargs rm -f
	rm ./tmp -rf
