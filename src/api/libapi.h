#ifndef LIBAPI_H_H
#define LIBAPI_H_H

void putchar(int str);
void puts(char *string);
void end_app();
int mkwindow(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void win_puts(int win, int x, int y, int col, int len, char * str);
void win_boxfill(int win, int x0, int y0, int x1, int y1, int col);

#endif
