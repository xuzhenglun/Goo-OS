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

char io_in8(short port){
	char date;
    __asm__ __volatile__(
        "in %0,%1"
		:"=a"(date)
        :"d"(port)
    );
	return date;
}

void io_out8(short port,char date){
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

int load_cr0(void){
    int cr0;
    __asm__ __volatile__(
        "MOV %0,CR0"
        :"=r"(cr0)
        );
    return cr0;
}

void store_cr0(int cr0){
    __asm__ __volatile__(
        "MOV CR0,%0":
        :"r"(cr0)
        );
}

void load_tr(short tr){
    __asm__ __volatile__(
        "LTR %0"
        :
        :"r"(tr)
        );
}

void farjump(int eip,int cs){
    __asm__ __volatile__(
            "JMP FAR PTR [esp+8]"
        );
}

void farcall(int eip,int cs){
    __asm__ __volatile__(
            "CALL FAR PTR [esp+8]"
        );
}

/*void start_app(int eip, int cs, int esp, int ds, int *tss_esp0){*/
    /*__asm__ __volatile__(*/
        /*"PUSHAD\n"*/
        /*"MOV    EAX,%0\n"*/
        /*"MOV    ECX,%1\n"*/
        /*"MOV    EDX,%2\n"*/
        /*"MOV    EBX,%3\n"*/
        /*"MOV    EBP,%4\n"*/
        /*"MOV    [EBP],ESP\n"*/
        /*"MOV    [EBP + 4],SS\n"*/
        /*"MOV    ES,BX\n"*/
        /*"MOV    DS,BX\n"*/
        /*"MOV    FS,BX\n"*/
        /*"MOV    GS,BX\n"*/

        /*"OR     ECX,3\n"*/
        /*"OR     EBX,3\n"*/
        /*"PUSH   EBX\n"*/
        /*"PUSH   EDX\n"*/
        /*"PUSH   ECX\n"*/
        /*"PUSH   EAX\n"*/
        /*"RETF\n"*/
        /*:*/
        /*:"r"(eip),"r"(cs),"r"(esp),"r"(ds),"r"(tss_esp0)*/
        /*);*/
/*}*/
