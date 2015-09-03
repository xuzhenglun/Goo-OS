#ifndef CONSOLE_H_H
#define CONSOLE_H_H

extern struct LAYER_CTL * layctl;
extern struct LAYER *lay_back,*lay_mouse,*lay_win;
extern struct MEMMAN *memman;
extern struct TIMERCTRL timerctrl;

#include "layer.h"

struct CONSOLE{
    struct LAYER *layer;
    int x,y;
};

void task_cons_main(struct LAYER *lay_win_b);
void cons_newline(struct CONSOLE *console);
int find_file(char *cmdline, int start);
void cons_runcmd(char *cmdline, struct CONSOLE *console);
void cons_mem(struct CONSOLE *console);
void cons_clear(struct CONSOLE *console);
void cons_ls(struct CONSOLE *console);
void cons_cat(char *cmdline, struct CONSOLE *console);
void cons_startapp(char *cmdline, struct CONSOLE *console);
void cons_print(struct CONSOLE *console, char *p, int filesize);
void cons_putchar(struct CONSOLE *console, int chr);
void cons_puts(struct CONSOLE *console, char *p);

#endif
