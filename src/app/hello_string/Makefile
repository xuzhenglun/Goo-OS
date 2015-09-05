OP = .
OBJS_BOOTPACK = libapi.o fw.o

INCPATH  = ./../api/ ./../golibc/

MAKE     = make
NASM     = nasm
CC       = gcc -O0 -I$(INCPATH) -c -std=c99 -masm=intel -fno-pic -static \
		   -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -funsigned-char \
		   -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -c
OBJCOPY  = objcopy

hello_s.go:
	$(NASM) -f elf32 ./hello_s.s -o hello_s.o
	$(OBJCOPY) --set-section-flags .bss=alloc,load,contents -O binary hello_s.o $(OP)/hello_s.go
