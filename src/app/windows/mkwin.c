int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void end_app(void);



void appmain(){
    char buf[150 * 50];
    api_openwin(buf, 150, 50, -1, "hello");
    end_app();
}
