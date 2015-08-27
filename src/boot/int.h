#ifndef INT_H_H
#define INT_H_H

#include "basic.h"
#include "fifo.h"


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

void init_pic(void);
void int_handler_20(int * esp);
void int_handler_21(int * esp);
void int_handler_27(int * esp);
void int_handler_2c(int * esp);
void asm_int_handler_20(void);
void asm_int_handler_21(void);
void asm_int_handler_27(void);
void asm_int_handler_2c(void);

#endif