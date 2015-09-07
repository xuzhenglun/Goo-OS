void api_end(void);

void appmain(void){
    *((char *) 0x00102600) = 0;
    end_app();
}
