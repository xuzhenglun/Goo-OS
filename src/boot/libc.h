#ifndef LIBC_H_H
#define LIBC_H_H
#endif

#include <sys/types.h>

void hlt(void);
void cli(void);
void io_out8(int16_t port,int8_t date);
int  io_load_eflags(void);
void io_store_eflags(int eflags);
