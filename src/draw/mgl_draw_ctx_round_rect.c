#include "mgl_draw_ctx_round_rect.h"
void mgl_ctx_fill_round_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_coord_t radius,const mgl_painter_t *painter){

    MGL_CHECK(w>=2*radius);
    MGL_CHECK(h>=2*radius);
    MGL_CHECK(radius>=0);

    if(radius==0){
        mgl_ctx_fill_rect(ctx,x,y,w,h,painter);
        return;
    }

    mgl_coord_t temp_w=(mgl_coord_t)(w-2*radius);
    mgl_coord_t temp_h=(mgl_coord_t)(h-2*radius);
    //中心矩形
    mgl_ctx_fill_rect(ctx,(mgl_coord_t)(x+radius),y,temp_w,h,painter);
    //左上角
    mgl_ctx_fill_arc(ctx,(mgl_coord_t)(x+radius),(mgl_coord_t)(y+radius),radius,180,270,painter);
    //右上角
    mgl_ctx_fill_arc(ctx,(mgl_coord_t)(x+w-radius),(mgl_coord_t)(y+radius),radius,270,0,painter);
    //左下角
    mgl_ctx_fill_arc(ctx,(mgl_coord_t)(x+radius),(mgl_coord_t)(y+h-radius),radius,90,180,painter);
    //右下角
    mgl_ctx_fill_arc(ctx,(mgl_coord_t)(x+w-radius),(mgl_coord_t)(y+h-radius),radius,0,90,painter);
    //左
    mgl_ctx_fill_rect(ctx,x,(mgl_coord_t)(y+radius),radius,temp_h,painter);
    //右
    mgl_ctx_fill_rect(ctx,(mgl_coord_t)(x+w-radius),(mgl_coord_t)(y+radius),radius,temp_h,painter);

}

void mgl_ctx_draw_round_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_coord_t radius,mgl_coord_t width,const mgl_painter_t *painter){

    MGL_CHECK(w>=2*radius);
    MGL_CHECK(h>=2*radius);
    MGL_CHECK(width>0);
    MGL_CHECK(w>=2*width);
    MGL_CHECK(h>=2*width);
    MGL_CHECK(radius>=0);


    if(radius==0){
        mgl_ctx_draw_rect(ctx,x,y,w,h,width,painter);
        return;
    }

    mgl_coord_t temp_w=(mgl_coord_t)(w-2*radius);
    mgl_coord_t temp_h=(mgl_coord_t)(h-2*radius);
    //左上角
    mgl_ctx_draw_arc(ctx,(mgl_coord_t)(x+radius),(mgl_coord_t)(y+radius),radius,width,180,270,painter);
    //右上角
    mgl_ctx_draw_arc(ctx,(mgl_coord_t)(x+w-radius),(mgl_coord_t)(y+radius),radius,width,270,0,painter);
    //左下角
    mgl_ctx_draw_arc(ctx,(mgl_coord_t)(x+radius),(mgl_coord_t)(y+h-radius),radius,width,90,180,painter);
    //右下角
    mgl_ctx_draw_arc(ctx,(mgl_coord_t)(x+w-radius),(mgl_coord_t)(y+h-radius),radius,width,0,90,painter);
    //上
    mgl_ctx_fill_rect(ctx,(mgl_coord_t)(x+radius),y,temp_w,width,painter);
    //左
    mgl_ctx_fill_rect(ctx,x,(mgl_coord_t)(y+radius),width,temp_h,painter);
    //下
    mgl_ctx_fill_rect(ctx,(mgl_coord_t)(x+radius),(mgl_coord_t)(y+h-width),temp_w,width,painter);
    //右
    mgl_ctx_fill_rect(ctx,(mgl_coord_t)(x+w-width),(mgl_coord_t)(y+radius),width,temp_h,painter);

}