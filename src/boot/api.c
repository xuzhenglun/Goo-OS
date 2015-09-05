#include "console.h"

void api_handler(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
    struct CONSOLE *cons = (struct CONSOLE *)*((int *)  0x0fec);
    int cs_base = *((int *)0xfe8);
    switch(edx){
        case 1:
            cons_putchar(cons,eax & 0xff);
            break;
        case 2:
            cons_puts(cons,(char *)ebx + cs_base);
            break;
        case 3:
            cons_print(cons,(char *)ebx + cs_base, ecx);
            break;
    }
}