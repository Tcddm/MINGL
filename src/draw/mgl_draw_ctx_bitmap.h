#ifndef MGL_DRAW_CTX_BITMAP_H
#define MGL_DRAW_CTX_BITMAP_H

#include "mgl_draw_ctx.h"

#ifdef __cplusplus
extern "C"{
#endif

void mgl_ctx_draw_bitmap_part(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,const mgl_rect_t *src_rect,const mgl_bitmap_t *bmp,mgl_color_t transparent_color);

static inline void mgl_ctx_draw_bitmap(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,const mgl_bitmap_t *bmp,mgl_color_t transparent_color){
    mgl_rect_t src_rect={0,0,bmp->w,bmp->h};
    mgl_ctx_draw_bitmap_part(ctx,x,y,&src_rect,bmp,transparent_color);
}

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_BITMAP_H
