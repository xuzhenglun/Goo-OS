#include "../../api/libapi.h"

void appmain(){
    char a[100];
    a[10] = 'A';
    putchar(a[10]);
    a[102] = 'B';
    putchar(a[102]);
    a[123] = 'C';
    putchar(a[123]);
    end_app();
}
