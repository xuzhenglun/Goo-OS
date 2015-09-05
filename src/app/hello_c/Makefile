OP = .
OBJS = fw.o libapi.o

API_PATH  = ./../../api/
LIBC_PATH = ./../../golibc/

MAKE     = make
NASM     = nasm
CC       = gcc -O0 -I$(INCPATH) -c -std=c99 -masm=intel -fno-pic -static \
		   -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -funsigned-char \
		   -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -c
OBJCOPY  = objcopy

hello_c.go: $(OBJS) hello_c.c Makefile
	$(CC) ./hello_c.c -o hello_c.o
	ld -m elf_i386 -o hello_c.obj $(OBJS) hello_c.o
	$(OBJCOPY) --set-section-flags .bss=alloc,load,contents -O binary hello_c.obj $(OP)/hello_c.go

%.o : $(API_PATH)/%.s Makefile
	$(NASM) -f elf32 $(API_PATH)/$*.s -o $*.o

%.o : $(API_PATH)/%.c Makefile
	$(CC) $(API_PATH)/$*.c

%.o : $(LIBC_PATH)/%.s Makefile
	$(NASM) -f elf32 $(LIBC_PATH)/$*.s -o $*.o

%.o : $(LIBC_PATH)/%.c Makefile
	$(CC) $(LIBC_PATH)/$*.c
