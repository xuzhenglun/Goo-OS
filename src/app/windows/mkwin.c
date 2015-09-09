#include "./../../api/libapi.h"

void appmain(){
    char buf[150 * 50];
    char title[30] = "Hello";
    mkwindow(buf, 150, 50, -1 , title);
    end_app();
}
