#include "mgl_draw_ctx.h"

mgl_draw_ctx_t g_mgl_full_screen_ctx;

void mgl_ctx_init(mgl_draw_ctx_t *ctx,mgl_widget_t *widget,const mgl_rect_t *clip){
    ctx->clip=*clip;
    ctx->widget=widget;
}
void mgl_ctx_draw_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_coord_t width,const mgl_painter_t *painter){

    MGL_CHECK(w>=width*2);
    MGL_CHECK(h>=width*2);

    int32_t rx = (int32_t)x + w - 1;
    int32_t ry = (int32_t)y + h - 1;
    //上
    mgl_ctx_fill_rect(ctx,x,y,w,width,painter);
    //下
    mgl_ctx_fill_rect(ctx,x,(mgl_coord_t)(ry-width+1),w,width,painter);
    //左
    mgl_ctx_fill_rect(ctx,x,(mgl_coord_t)(y+width),width,(mgl_coord_t)(h-2*width),painter);
    //右
    mgl_ctx_fill_rect(ctx,(mgl_coord_t)(rx-width+1),(mgl_coord_t)(y+width),width,(mgl_coord_t)(h-2*width),painter);
}
void mgl_ctx_fill_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,const mgl_painter_t *painter){
    mgl_rect_t rect={x,y,w,h};
    mgl_rect_t draw_area;
    if (mgl_rect_intersect(&rect,&ctx->clip,&draw_area)){
        if(painter->type==MGL_PAINTER_TYPE_SOLID){
            mgl_hal_fill_rect(draw_area.x,draw_area.y,draw_area.w,draw_area.h,painter->solid_color);
        }else if(painter->type==MGL_PAINTER_TYPE_CUSTOM){
            painter->vtable->fill_rect(painter,draw_area.x,draw_area.y,draw_area.w,draw_area.h);
        }
    }
}

void mgl_ctx_draw_line(mgl_draw_ctx_t *ctx,mgl_coord_t x0,mgl_coord_t y0,mgl_coord_t x1,mgl_coord_t y1,mgl_coord_t w,const mgl_painter_t *painter){

    MGL_CHECK(w>0);

    mgl_coord_t half=(mgl_coord_t)(w/2);
    mgl_coord_t dx=(mgl_coord_t)(x1-x0);
    mgl_coord_t dy=(mgl_coord_t)(y1-y0);
    mgl_coord_t abs_dx= MGL_ABS_I16(dx);
    mgl_coord_t abs_dy= MGL_ABS_I16(dy);

    if(abs_dx==0&&abs_dy==0){
        mgl_ctx_fill_rect(ctx,(mgl_coord_t)(x0-half),(mgl_coord_t)(y0-half),w,w,painter);
        return;
    }
    if(abs_dx==0){
        mgl_coord_t x=(mgl_coord_t)(x0-half);
        mgl_coord_t y=(mgl_coord_t)((dy>0)?y0:y1);
        mgl_coord_t h=(mgl_coord_t)(abs_dy+1);
        mgl_ctx_fill_rect(ctx,x,y,w,h,painter);
        return;
    }
    if(abs_dy==0){
        mgl_coord_t y=(mgl_coord_t)(y0-half);
        mgl_coord_t x=(mgl_coord_t)((dx>0)?x0:x1);
        mgl_coord_t wd=(mgl_coord_t)(abs_dx+1);
        mgl_ctx_fill_rect(ctx,x,y,wd,w,painter);
        return;
    }

    mgl_coord_t sx=(dx>0)?1:-1;
    mgl_coord_t sy=(dy>0)?1:-1;
    int32_t err=abs_dx-abs_dy;
    if(abs_dx>abs_dy){
        //X为主轴
        mgl_coord_t x=x0;
        mgl_coord_t y=y0;

        while(1){
            mgl_ctx_fill_rect(ctx,x,(mgl_coord_t)(y-half),1,w,painter);
            if(x==x1) break;
            int32_t e2=err*2;
            if(e2>-abs_dy){
                err=err-abs_dy;
                x=(mgl_coord_t)(x+sx);
            }
            if(e2<abs_dx){
                err=err+abs_dx;
                y=(mgl_coord_t)(y+sy);
            }
        }
    }else{
        //Y为主轴
        mgl_coord_t x=x0;
        mgl_coord_t y=y0;

        while(1){
            mgl_ctx_fill_rect(ctx,(mgl_coord_t)(x-half),y,w,1,painter);
            if(y==y1) break;
            int32_t e2=err*2;
            if(e2>-abs_dy){
                err=err-abs_dy;
                x=(mgl_coord_t)(x+sx);
            }
            if(e2<abs_dx){
                err=err+abs_dx;
                y=(mgl_coord_t)(y+sy);
            }
        }
    }

}

