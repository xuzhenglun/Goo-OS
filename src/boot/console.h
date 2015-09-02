#ifndef CONSOLE_H_H
#define CONSOLE_H_H

extern struct LAYER_CTL * layctl;
extern struct LAYER *lay_back,*lay_mouse,*lay_win;
extern struct MEMMAN *memman;
extern struct TIMERCTRL timerctrl;

void task_cons_main(struct LAYER *lay_win_b);
int cons_newline(int y, struct LAYER *layer);
int find_file(char *cmdline, int start);

#endif
