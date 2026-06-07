#include "mgl_draw_ctx_arc.h"
static void mgl_arc_get_xy(mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,uint16_t angle,mgl_coord_t *x,mgl_coord_t *y){
    *x=(mgl_coord_t)(cx+((int32_t)radius*mgl_cos_i16((int16_t)angle)>>15));
    *y=(mgl_coord_t)(cy+((int32_t)radius*mgl_sin_i16((int16_t)angle)>>15));
}
static uint16_t mgl_arc_get_span(uint16_t start_angle,uint16_t end_angle){
    int16_t span=(int16_t)(end_angle-start_angle);
    if (span > 360) span -= 360;
    if (span < 0) span += 360;
    if (span == 0) span = 360;
    return (uint16_t)span;
}
void mgl_ctx_draw_arc(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,mgl_coord_t w,uint16_t start_angle,uint16_t end_angle,const mgl_painter_t *painter){

    MGL_CHECK(radius>0);
    MGL_CHECK(w>0);
    MGL_CHECK(radius>w);

    uint16_t span=mgl_arc_get_span(start_angle,end_angle);

    mgl_coord_t last_x,last_y,last_in_x,last_in_y;

    mgl_arc_get_xy(cx,cy,radius,start_angle,&last_x,&last_y);
    mgl_arc_get_xy(cx,cy,(mgl_coord_t)(radius-w),start_angle,&last_in_x,&last_in_y);

    mgl_coord_t points[8];
    for(uint16_t i=1;i<=span;i++){

        uint16_t angle=start_angle+i;
        if(angle>=360){angle-=360;}

        mgl_coord_t x, y, in_x, in_y;
        mgl_arc_get_xy(cx,cy,radius,angle,&x,&y);
        mgl_arc_get_xy(cx,cy,(mgl_coord_t)(radius-w),angle,&in_x,&in_y);

        points[0]=last_x;
        points[1]=last_y;
        points[2]=last_in_x;
        points[3]=last_in_y;
        points[4]=in_x;
        points[5]=in_y;
        points[6]=x;
        points[7]=y;

        mgl_ctx_fill_polygon(ctx, points, 4, painter);


        last_x=x;
        last_y=y;
        last_in_x=in_x;
        last_in_y=in_y;
    }
}
void mgl_ctx_fill_arc(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,uint16_t start_angle,uint16_t end_angle,const mgl_painter_t *painter){

    MGL_CHECK(radius>0);

    uint16_t span=mgl_arc_get_span(start_angle,end_angle);

    mgl_coord_t last_x,last_y;

    mgl_arc_get_xy(cx,cy,radius,start_angle,&last_x,&last_y);

    mgl_coord_t points[6];
    for(uint16_t i=1;i<=span;i++){

        uint16_t angle=start_angle+i;
        if(angle>=360){angle-=360;}

        mgl_coord_t x, y;
        mgl_arc_get_xy(cx,cy,radius,angle,&x,&y);

        points[0]=cx;
        points[1]=cy;
        points[2]=last_x;
        points[3]=last_y;
        points[4]=x;
        points[5]=y;

        mgl_ctx_fill_polygon(ctx, points, 3, painter);


        last_x=x;
        last_y=y;
    }
}