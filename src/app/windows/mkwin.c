#include "./../../api/libapi.h"
#include "./../../golibc/stdio.h"

void appmain(){
    char buf[150 * 50];
    char title[30] = "Hello";
    int win;
    win = mkwindow(buf, 150, 50, -1 , title);
    win_boxfill(win, 8, 36, 141, 43, 3);
    win_puts(win, 28, 28, 0, 12, title);
    end_app();
}
