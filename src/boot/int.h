#define PORT_KEYDAT     0x0060

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1


#ifndef INT_H_H
#define INT_H_H
#endif

#define     PORT_KEYDAT         0x0060
#define     PORT_KEYSTA         0x0064
#define     PORT_KEYCMD         0x0064
#define     KEYSTA_SEND_NOTREADY 0x02
#define     KEYCMD_WRITE_MODE   0x60
#define     KBC_MODE            0x47
#define     KEYCMD_SENDTO_MOUSE 0xd4
#define     MOUSECMD_ENABLE     0xf4

struct MOUSE_DEC{
    unsigned char buf[3],phase;
    int x,y,btn;
};


int  mouse_decode(struct MOUSE_DEC *mdec,unsigned char dat);
void wait_KBC_sendready(void);
void init_keyboard(void);
void enable_mouse(struct MOUSE_DEC *mdec);



#ifndef BASIC_H_H
#define BASIC_H_H
#endif

#include "basic.h"

#include "fifo.h"
struct FIFO8 keyfifo,mousefifo;

void init_pic(void);
void int_handler_21(int * esp);
void int_handler_27(int * esp);
void int_handler_2c(int * esp);
void asm_int_handler_21(void);
void asm_int_handler_27(void);
void asm_int_handler_2c(void);
