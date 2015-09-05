OBJS_BOOTPACK = libapi.o fw.o

APPS = hlt.go hello.go hello2.go helloc.go

INCPATH  = ./../api/ ./../golibc/
OP = $(shell pwd)

MAKE     = make
NASM     = nasm
CC       = gcc -O0 -I$(INCPATH) -c -std=c99 -masm=intel -fno-pic -static \
		   -fno-builtin -fno-strict-aliasing -Wall -MD -ggdb -m32 -funsigned-char \
		   -fno-omit-frame-pointer -fno-stack-protector -fno-pic -O -c
OBJCOPY  = objcopy

SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')

all:$(SUBDIRS)

$(SUBDIRS):ECHO
	make -C $@ OP=$(OP)

ECHO:
	@echo $(SUBDIRS)
