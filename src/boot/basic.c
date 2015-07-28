#include "basic.h"

void hlt(void){
    __asm__ __volatile__("hlt");
}

void cli(void){
    __asm__ __volatile__("cli");
}

void sti(void){
    __asm__ __volatile__("sti");
}

char io_in8(int16_t port){
	char date;
    __asm__ __volatile__(
        "in %0,%1"
		:"=a"(date)
        :"d"(port)
    );
	return date;
}

void io_out8(int16_t port,int8_t date){
    __asm__ __volatile__(
        "out %0,%1":
        :"d"(port),"a"(date)
    );
}

int io_load_eflags(void){
    int eflags=0;
    __asm__ __volatile__(
        "pushfd;"
        "pop %0;"
        :"=r"(eflags)
    );
    return eflags;
}

void io_store_eflags(int eflags){
    __asm__ __volatile__(
        "PUSH %0;"
        "POPFD;":
        :"r"(eflags)
    );
}

void stihlt(void){
    __asm__ __volatile__(
        "STI\n\r"
        "HLT"
    );
}

