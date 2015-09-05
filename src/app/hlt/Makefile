OP = .
OBJS_BOOTPACK = libapi.o fw.o

INCPATH  = ./../api/ ./../golibc/

MAKE     = make
NASM     = nasm
CC       = gcc -O0 -I$(INCPATH) -c -std=c99 -masm=intel -fno-pic -static \
		   -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -funsigned-char \
		   -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -c
OBJCOPY  = objcopy

hlt.go:
	$(NASM) ./hlt.s -o $(OP)/hlt.go	
