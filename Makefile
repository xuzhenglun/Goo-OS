goo.img : os.img bootblock.bin
	rm ./tmp -rf
	mkdir ./tmp
	sudo mount -o loop os.img ./tmp
	sudo cp bootblock.bin ./tmp/bootblock.sys
	sudo umount ./tmp
	mv os.img goo.img

black.bin : ./src/app/black.s
	nasm ./src/app/black.s -o black.bin

IPL.bin : ./src/boot/IPL.s
	nasm ./src/boot/IPL.s -o IPL.bin

os.img : IPL.bin
	cp IPL.bin os.img
	dd if=/dev/zero of=os.img bs=512 seek=1 count=2879

bootblock.bin : bootblock.o
	objcopy  -O binary bootblock.o bootblock.bin

basic.o : ./src/boot/basic.c
	gcc -m32 -masm=intel  -c ./src/boot/basic.c

bootblock.o : bootasm.o bootmain.o basic.o font.o sprintf.o vsprintf.o strtoul0.o strlen.o dsctbl.o graph.o
	ld  -m elf_i386 -e start -Ttext 0xc400 -o bootblock.o bootasm.o bootmain.o basic.o font.o sprintf.o vsprintf.o strtoul0.o strlen.o dsctbl.o graph.o

bootasm.o : ./src/boot/bootasm.s
	nasm -f elf32 ./src/boot/bootasm.s -o bootasm.o

#io_out8.o : ./src/boot/io_out8.s
	#nasm -f elf32 ./src/boot/io_out8.s -o io_out8.o

bootmain.o : ./src/boot/bootmain.c
	gcc -c -std=c99 -fno-pic -static -lc -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -I. -c ./src/boot/bootmain.c
	#gcc -c -std=c99 -m32  ./src/boot/bootmain.c

makefont.a : ./tools/makefont.c
	gcc -m32  ./tools/makefont.c -o makefont.a

font.o : font.bin bin2obj.a
	#./bin2obj.a fontbin.tmp  font.o _hankaku
	objcopy -I binary -O elf32-i386 -B i386 font.bin font.o

font.bin : makefont.a ./src/boot/hankaku.txt
	./makefont.a ./src/boot/hankaku.txt font.bin

bin2obj.a : ./tools/bin2obj.c
	gcc -m32 ./tools/bin2obj.c -o bin2obj.a
	
%.o : ./src/golibc/%.c
	gcc -m32 -std=c99  -masm=intel -c ./src/golibc/$*.c
	
%.o : ./src/boot/%.c
	gcc -m32 -std=c99  -masm=intel -c ./src/boot/$*.c

debug : goo.img
	qemu-system-i386 -boot order=a -fda ./goo.img -m 8 -S -s

run : goo.img
	qemu-system-i386 -boot order=a -fda ./goo.img -m 256

clean :
	#sudo umount ./tmp
	find . -name "*.img"  | xargs rm -f
	find . -name "*.o"  | xargs rm -f
	find . -name "*.d"  | xargs rm -f
	find . -name "*.a"  | xargs rm -f
	find . -name "*.bin"  | xargs rm -f
	rm ./tmp -rf
