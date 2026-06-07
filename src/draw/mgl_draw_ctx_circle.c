#include "mgl_draw_ctx_circle.h"

static void mgl_ctx_draw_circle_1px(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,const mgl_painter_t *painter){
    const mgl_rect_t *clip=&ctx->clip;

    const mgl_coord_t x1=(mgl_coord_t)(cx-radius);
    const mgl_coord_t y1=(mgl_coord_t)(cy-radius);
    const mgl_coord_t x2=(mgl_coord_t)(cx+radius);
    const mgl_coord_t y2=(mgl_coord_t)(cy+radius);

    if(x2<clip->x||x1>=clip->x+clip->w) return;
    if(y2<clip->y||y1>=clip->y+clip->h) return;


    const bool fully_inside=(x1>=clip->x
                             &&y1>=clip->y
                             &&x2<clip->x+clip->w
                             &&y2<clip->y+clip->h);


    mgl_coord_t x=0;
    mgl_coord_t y=(mgl_coord_t)radius;
    int32_t d=(int32_t)(3-2*radius);
    if(fully_inside&&painter->type==MGL_PAINTER_TYPE_SOLID){
        mgl_color_t color=painter->solid_color;
        while(x<=y){
            mgl_hal_set_pixel((mgl_coord_t)(cx+x),(mgl_coord_t)(cy+y),color);
            mgl_hal_set_pixel((mgl_coord_t)(cx+y),(mgl_coord_t)(cy+x),color);
            mgl_hal_set_pixel((mgl_coord_t)(cx-x),(mgl_coord_t)(cy+y),color);
            mgl_hal_set_pixel((mgl_coord_t)(cx-y),(mgl_coord_t)(cy+x),color);
            mgl_hal_set_pixel((mgl_coord_t)(cx-x),(mgl_coord_t)(cy-y),color);
            mgl_hal_set_pixel((mgl_coord_t)(cx-y),(mgl_coord_t)(cy-x),color);
            mgl_hal_set_pixel((mgl_coord_t)(cx+x),(mgl_coord_t)(cy-y),color);
            mgl_hal_set_pixel((mgl_coord_t)(cx+y),(mgl_coord_t)(cy-x),color);
            if(d<0){
                d=d+4*x+6;
            }else{
                d=d+4*(x-y)+10;
                y--;
            }
            x++;
        }
    }else{
        while(x<=y){
            mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(cx+x),(mgl_coord_t)(cy+y),painter);
            mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(cx+y),(mgl_coord_t)(cy+x),painter);
            mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(cx-x),(mgl_coord_t)(cy+y),painter);
            mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(cx-y),(mgl_coord_t)(cy+x),painter);
            mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(cx-x),(mgl_coord_t)(cy-y),painter);
            mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(cx-y),(mgl_coord_t)(cy-x),painter);
            mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(cx+x),(mgl_coord_t)(cy-y),painter);
            mgl_ctx_draw_pixel(ctx,(mgl_coord_t)(cx+y),(mgl_coord_t)(cy-x),painter);
            if(d<0){
                d=d+4*x+6;
            }else{
                d=d+4*(x-y)+10;
                y--;
            }
            x++;
        }
    }
}

static void mgl_ctx_fill_circle_quick(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,const mgl_painter_t *painter){
    const mgl_rect_t *clip=&ctx->clip;

    const mgl_coord_t x1=(mgl_coord_t)(cx-radius);
    const mgl_coord_t y1=(mgl_coord_t)(cy-radius);
    const mgl_coord_t x2=(mgl_coord_t)(cx+radius);
    const mgl_coord_t y2=(mgl_coord_t)(cy+radius);

    if(x2<clip->x||x1>=clip->x+clip->w) return;
    if(y2<clip->y||y1>=clip->y+clip->h) return;



    mgl_coord_t x=0;
    mgl_coord_t y=(mgl_coord_t)radius;
    int32_t d=3-2*radius;

    while(x<=y){
        mgl_ctx_draw_line(ctx,(mgl_coord_t)(cx-x),(mgl_coord_t)(cy+y),(mgl_coord_t)(cx+x),(mgl_coord_t)(cy+y),1,painter);
        mgl_ctx_draw_line(ctx,(mgl_coord_t)(cx-x),(mgl_coord_t)(cy-y),(mgl_coord_t)(cx+x),(mgl_coord_t)(cy-y),1,painter);
        mgl_ctx_draw_line(ctx,(mgl_coord_t)(cx-y),(mgl_coord_t)(cy+x),(mgl_coord_t)(cx+y),(mgl_coord_t)(cy+x),1,painter);
        mgl_ctx_draw_line(ctx,(mgl_coord_t)(cx-y),(mgl_coord_t)(cy-x),(mgl_coord_t)(cx+y),(mgl_coord_t)(cy-x),1,painter);
        if(d<0){
            d=d+4*x+6;
        }else{
            d=d+4*(x-y)+10;
            y--;
        }
        x++;
    }
}

void mgl_ctx_draw_circle(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,mgl_coord_t w,const mgl_painter_t *painter){

    MGL_CHECK(radius>0);
    MGL_CHECK(w>0);

    if(w==1){
        mgl_ctx_draw_circle_1px(ctx,cx,cy,radius,painter);
    }else{
        mgl_ctx_draw_arc(ctx,cx,cy,radius,w,0,360,painter);
    }
}

void mgl_ctx_fill_circle(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,bool quick,const mgl_painter_t *painter){

    MGL_CHECK(radius>0);

    if(quick){
        mgl_ctx_fill_circle_quick(ctx,cx,cy,radius,painter);
    }else{
        mgl_ctx_fill_arc(ctx,cx,cy,radius,0,360,painter);
    }
}