#include "mgl_rect.h"

bool mgl_rect_intersect(const mgl_rect_t *a,const mgl_rect_t *b,mgl_rect_t *out){
    int32_t x1=(a->x > b->x) ? a->x : b->x;
    int32_t y1=(a->y > b->y) ? a->y : b->y;
    int32_t x2=((int32_t)a->x+a->w) < ((int32_t)b->x+b->w) ? ((int32_t)a->x+a->w) : ((int32_t)b->x+b->w);
    int32_t y2=((int32_t)a->y+a->h) < ((int32_t)b->y+b->h) ? ((int32_t)a->y+a->h) : ((int32_t)b->y+b->h);
    if (x1<x2&&y1<y2){
        out->x=(mgl_coord_t)x1;
        out->y=(mgl_coord_t)y1;
        out->w=(mgl_coord_t)(x2-x1);
        out->h=(mgl_coord_t)(y2-y1);
        return true;
    }
    return false;
}

void mgl_rect_union(const mgl_rect_t *a,const mgl_rect_t *b,mgl_rect_t *out){
    int32_t left=(a->x < b->x) ? a->x : b->x;
    int32_t top=(a->y < b->y) ? a->y : b->y;
    int32_t right=((int32_t)a->x+a->w) > ((int32_t)b->x+b->w) ? ((int32_t)a->x+a->w) : ((int32_t)b->x+b->w);
    int32_t bottom=((int32_t)a->y+a->h) > ((int32_t)b->y+b->h) ? ((int32_t)a->y+a->h) : ((int32_t)b->y+b->h);
    out->x=(mgl_coord_t)left;
    out->y=(mgl_coord_t)top;
    out->w=(mgl_coord_t)(right-left);
    out->h=(mgl_coord_t)(bottom-top);
}