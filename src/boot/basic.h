#ifndef BASIC_H_H
#define BASIC_H_H

void hlt(void);
void cli(void);
void sti(void);
char io_in8(short port);
void io_out8(short port,char date);
int  io_load_eflags(void);
void io_store_eflags(int eflags);
void int_test(void);
void stihlt(void);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(short tr);
void farjump(int eip,int cs);
void farcall(int eip,int cs);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);

#endif
