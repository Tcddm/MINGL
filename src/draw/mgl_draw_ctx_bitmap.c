#include "mgl_draw_ctx_bitmap.h"
void mgl_ctx_draw_bitmap_part(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,const mgl_rect_t *src_rect,const mgl_bitmap_t *bmp,mgl_color_t transparent_color){
    mgl_rect_t dst={x,y,src_rect->w,src_rect->h};
    mgl_rect_t draw_area;
    if(!mgl_rect_intersect(&dst,&ctx->clip,&draw_area)){ return;}
    mgl_rect_t final_src={
            .x=(mgl_coord_t)(src_rect->x+(draw_area.x-x)),
            .y=(mgl_coord_t)(src_rect->y+(draw_area.y-y)),
            .w=draw_area.w,
            .h=draw_area.h
    };
    
    //源x不能小于0
    if (final_src.x<0) {
        final_src.w=(mgl_coord_t)(final_src.w+final_src.x);
        final_src.x=0;
    }

    //源y不能小于0
    if (final_src.y<0) {
        final_src.h=(mgl_coord_t)(final_src.h+final_src.y);
        final_src.y=0;
    }

    MGL_CHECK(final_src.x<bmp->w&&final_src.y<bmp->h);

    //源宽度不能超出位图右边界
    if (final_src.x+final_src.w>bmp->w) {
        final_src.w=(mgl_coord_t)(bmp->w-final_src.x);
    }

    //源高度不能超出位图下边界
    if (final_src.y+final_src.h>bmp->h) {
        final_src.h=(mgl_coord_t)(bmp->h-final_src.y);
    }

    //裁剪后宽高<=0直接不绘制
    MGL_GUARD_CHECK(final_src.w>0&&final_src.h>0);

    mgl_hal_bit_blt(draw_area.x,draw_area.y,&final_src,bmp,transparent_color);
}