OP = .
OBJS = libapi.o

API_PATH  = ./../../api/
LIBC_PATH = ./../../golibc/

MAKE     = make
NASM     = nasm
CC       = gcc -O0 -I$(INCPATH) -c -std=c99 -masm=intel -fno-pic -static \
		   -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -funsigned-char \
		   -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -c
OBJCOPY  = objcopy

mkw.go: $(OBJS) mkwin.c mkw.s Makefile
	$(CC) mkwin.c
	$(NASM) -f elf32 mkw.s -o mkw.o
	ld -m elf_i386 -o mkwin.obj mkwin.o mkw.o libapi.o
	$(OBJCOPY) --set-section-flags .bss=alloc,load,contents -O binary mkwin.obj $(OP)/mkw.go

%.o : $(API_PATH)/%.s Makefile
	$(NASM) -f elf32 $(API_PATH)/$*.s -o $*.o

%.o : $(API_PATH)/%.c Makefile
	$(CC) $(API_PATH)/$*.c

%.o : $(LIBC_PATH)/%.s Makefile
	$(NASM) -f elf32 $(LIBC_PATH)/$*.s -o $*.o

%.o : $(LIBC_PATH)/%.c Makefile
	$(CC) $(LIBC_PATH)/$*.c
