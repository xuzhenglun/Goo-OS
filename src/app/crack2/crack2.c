void end_app(void);

void appmain(void){
    *((char *) 0x00102600) = 0;
    end_app();
}
