OBJS_BOOTPACK = bootasm.o bootmain.o basic.o font.o sprintf.o vsprintf.o strtoul0.o strlen.o dsctbl.o graph.o int.o int_asm.o fifo.o memory.o layer.o timer.o mtask.o keyboard.o strcmp.o memcmp.o memcpy.o strncmp.o fat12.o

INCPATH  = ./src/golibc/

MAKE     = make
NASM     = nasm
CC       = gcc -O0 -I$(INCPATH) -c -std=c99 -masm=intel -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -funsigned-char -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -c
OBJCOPY  = objcopy


goo.img : os.img bootblock.bin Makefile
	rm ./tmp -rf
	mkdir ./tmp
	sudo mount -o loop os.img ./tmp
	sudo cp bootblock.bin ./tmp/bootblock.sys
	echo "God's in his heaven. All's right with the world!" > ./NERV.txt
	sudo cp ./NERV.txt ./tmp/NERV.txt
	rm ./NERV.txt
	sudo cp ./src/boot/IPL.s ./tmp/
	sudo cp ./src/boot/basic.c ./tmp/
	sudo umount ./tmp
	mv os.img goo.img

black.bin : ./src/app/black.si Makefile
	$(NASM) ./src/app/black.s -o black.bin

IPL.bin : ./src/boot/IPL.s Makefile
	$(NASM) ./src/boot/IPL.s -o IPL.bin
	
bootasm.o : ./src/boot/bootasm.s Makefile
	$(NASM) -f elf32 ./src/boot/bootasm.s -o bootasm.o

int_asm.o : ./src/boot/int_asm.s Makefile
	$(NASM) -f elf32 ./src/boot/int_asm.s -o int_asm.o

#io_out8.o : ./src/boot/io_out8.s
	#$(NASM) -f elf32 ./src/boot/io_out8.s -o io_out8.o

os.img : IPL.bin Makefile
	cp IPL.bin os.img
	dd if=/dev/zero of=os.img bs=512 seek=1 count=2879

bootblock.bin : bootblock.o Makefile
	$(OBJCOPY) --set-section-flags .bss=alloc,load,contents -O binary bootblock.o bootblock.bin

bootblock.o : $(OBJS_BOOTPACK) Makefile
	ld  -m elf_i386 -e start -Ttext 0xc400 -o bootblock.o $(OBJS_BOOTPACK)

makefont.a : ./tools/makefont.c Makefile
	gcc  ./tools/makefont.c -o makefont.a

font.o : font.bin Makefile
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 font.bin font.o

font.bin : makefont.a ./src/boot/hankaku.txt Makefile
	./makefont.a ./src/boot/hankaku.txt font.bin

%.o : ./src/golibc/%.c Makefile
	$(CC) ./src/golibc/$*.c
	
%.o : ./src/boot/%.c ./src/boot/%.h Makefile
	$(CC) ./src/boot/$*.c

debug : goo.img
	qemu-system-i386 -boot order=a -fda ./goo.img -m 8 -S -s -monitor stdio

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

travis: bootblock.bin os.img
