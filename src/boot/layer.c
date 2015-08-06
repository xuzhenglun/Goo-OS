#include "layer.h"

struct LAYER_CTL * layer_ctl_init(struct MEMMAN *man, unsigned char *vram, int xsize ,int ysize){
    struct LAYER_CTL *ctl;
    ctl = (struct LAYER_CTL *) mem_alloc_4k(man, sizeof(struct LAYER_CTL));
    if(ctl == 0){
        return ctl;
    }
    ctl->vram = vram;
    ctl->xsize = xsize;
    ctl->ysize = ysize;
    ctl->top = -1;
    for( int i = 0; i < MAX_LAYERS; i++ ){
        ctl->layers[i].flags = 0;
        ctl->layers[i].ctl   = ctl;
    }
    return ctl;
}

struct LAYER * layer_alloc(struct LAYER_CTL *ctl){
    struct LAYER *lay;
    for(int i = 0; i < MAX_LAYERS; i++){
        if(ctl->layers[i].flags == 0){
            lay = &ctl->layers[i];
            lay->flags = LAYER_USE;
            lay->height = -1;
            return lay;
        }
    }
    return 0;
}

void layer_setbuf(struct LAYER *lay, unsigned char *buf, int xsize, int ysize, int col_inv){
    lay->buf = buf;
    lay->bxsize = xsize;
    lay->bysize = ysize;
    lay->col_inv = col_inv;
    return;
}

void layer_updown(struct LAYER * lay, int height){
    struct LAYER_CTL *ctl = lay->ctl;
    int h,old = lay->height;

    if(height > ctl->top + 1){
        height = ctl->top + 1;
    }
    if(height < -1){
         height = -1;
    }
    lay->height = height;

    if(old > height){
        if(height >= 0){
            for(h = old; h > height; h--){
                ctl->layers_p[h] = ctl->layers_p[h - 1];
                ctl->layers_p[h]->height = h;
            }
            ctl->layers_p[height] = lay;
        }else{
            if(ctl->top > old){
                for(h = old; h < ctl->top; h++){
                    ctl->layers_p[h] = ctl->layers_p[h + 1];
                    ctl->layers_p[h]->height = h;
                }
            }
            ctl->top--;
        }
        layer_refresh_sub(lay->ctl,lay->vx0,lay->vy0,lay->vx0 + lay->bxsize, lay->vy0 + lay->bysize);
    }else if(old < height){
        if(old >= 0){
            for(h = old; h < height; h++){
                ctl->layers_p[h] = ctl->layers_p[h + 1];
                ctl->layers_p[h]->height = h;
            }
        ctl->layers_p[height] = lay;
        }else{
            for(h = ctl->top; h >= height; h--){
                ctl->layers_p[h + 1] = ctl->layers_p[h];
                ctl->layers_p[h + 1]->height = h + 1;
            }
            ctl->layers_p[height] = lay;
            ctl->top++;
        }
        layer_refresh_sub(lay->ctl,lay->vx0,lay->vy0,lay->vx0 + lay->bxsize, lay->vy0 + lay->bysize);
    }
    return;
}

void layer_refresh(struct LAYER * lay, int bx0, int by0, int bx1, int by1){
    if( lay->height >= 0 ){
        layer_refresh_sub(lay->ctl, lay->vx0 + bx0, lay->vy0 + by0, lay->vx0 + bx1 + 1, lay->vy0 + by1 + 1);
    }
}

void layer_slide(struct LAYER * lay, int vx0, int vy0){
     int old_vx0 = lay->vx0;
     int old_vy0 = lay->vy0;
     lay->vx0 = vx0;
     lay->vy0 = vy0;
     if(lay->height >= 0){
         layer_refresh_sub(lay->ctl,old_vx0,old_vy0,old_vx0 + lay->bxsize,old_vy0 + lay->bysize);
         layer_refresh_sub(lay->ctl,vx0,vy0,vx0 + lay->bxsize, vy0 + lay->bysize);
     }
}

void layer_free(struct LAYER *lay){
    if(lay->height >= 0){
        layer_updown(lay,-1);
    }
    lay->flags = 0;
}

void layer_refresh_sub(struct LAYER_CTL *ctl, int vx0, int vy0, int vx1, int vy1){
     struct LAYER *lay;
     unsigned char * buf,c,*vram = ctl->vram;
	 if (vx0 < 0) vx0 = 0;
	 if (vy0 < 0) vy0 = 0;
	 if (vx1 > ctl->xsize) vx1 = ctl->xsize;
	 if (vy1 > ctl->ysize) vy1 = ctl->ysize;
     for(int h = 0;h <= ctl->top;h++){
         lay = ctl->layers_p[h];
         buf = lay->buf;
         int bx0 = vx0 - lay->vx0;
         int by0 = vy0 - lay->vy0;
         int bx1 = vx1 - lay->vx0;
         int by1 = vy1 - lay->vy0;
         if(bx0 < 0)    bx0 = 0;
         if(by0 < 0)    by0 = 0;
         if(bx1 > lay->bxsize)    bx1 = lay->bxsize;
         if(by1 > lay->bysize)    by1 = lay->bysize;

         for(int by = by0;by < by1; by++){
             int vy = lay->vy0 + by;
             for(int bx = bx0; bx < bx1; bx++){
                 int vx = lay->vx0 + bx;
                 c = buf[by * lay->bxsize + bx];
                 if( c != lay->col_inv ){
                    vram[vy * ctl->xsize + vx] = c;
                 }
             }
         }
     }
}
