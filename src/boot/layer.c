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

void layer_updown(struct LAYER_CTL * ctl, struct LAYER * lay, int height){
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
        layer_refresh(ctl);
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
        layer_refresh(ctl);
    }
    return;
}

void layer_refresh(struct LAYER_CTL *ctl){
     int h,bx,by,vx,vy;
     unsigned char *buf, c, *vram = ctl->vram;
     struct LAYER *lay;
     for(h = 0; h <= ctl->top; h++){
         lay = ctl->layers_p[h];
         buf = lay->buf;
         for(by = 0; by < lay->bysize; by++){
              vy = lay->vy0 + by;
              for(bx = 0;bx < lay->bxsize; bx++){
                  vx = lay->vx0 + bx;
                  c  = buf[by * lay->bxsize + bx];
                  if( c != lay->col_inv){
                      vram[vy * ctl->xsize + vx] = c;
                  }
              }
         }
     }
}

void layer_slide(struct LAYER_CTL *ctl,struct LAYER * lay, int vx0, int vy0){
     lay->vx0 = vx0;
     lay->vy0 = vy0;
     if(lay->height >= 0){
         layer_refresh(ctl);
     }
}

void layer_free(struct LAYER_CTL *ctl, struct LAYER *lay){
    if(lay->height >= 0){
        layer_updown(ctl,lay,-1);
    }
    lay->flags = 0;
}
