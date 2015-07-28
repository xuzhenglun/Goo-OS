#ifndef BASIC_H_H
#define BASIC_H_H
#endif

#include <sys/types.h>

void hlt(void);
void cli(void);
void sti(void);
char io_in8(int16_t port);
void io_out8(int16_t port,int8_t date);
int  io_load_eflags(void);
void io_store_eflags(int eflags);
void int_test(void);
void stihlt(void);
