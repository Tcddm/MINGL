#include "mgl_draw_ctx_fill_polygon.h"

void mgl_ctx_fill_polygon(mgl_draw_ctx_t *ctx,const mgl_coord_t * poly,uint16_t cnt,const mgl_painter_t *painter){

    MGL_CHECK(cnt>=3);

    mgl_coord_t y_min=poly[1];
    mgl_coord_t y_max=poly[1];
    for (uint16_t i=0;i<cnt;i++){
        mgl_coord_t y=poly[i*2+1];
        if (y<y_min) y_min=y;
        if (y>y_max) y_max=y;
    }

    mgl_coord_t xi[MGL_FILL_POLYGON_MAX_INTERSECTS];


    for (mgl_coord_t y=y_min;y<=y_max;y++) {
        mgl_coord_t n=0;

        for (uint16_t i=0;i<cnt;i++){
            uint16_t j=(i+1)%cnt;

            mgl_coord_t x1=poly[i*2+0];
            mgl_coord_t y1=poly[i*2+1];
            mgl_coord_t x2=poly[j*2+0];
            mgl_coord_t y2=poly[j*2+1];

            //跳过水平边
            if (y1==y2) {
                continue;
            }

            //判断边是否跨越当前扫描线
            if ((y1>y)!=(y2>y)){
                //求交点
                int32_t dx=(int32_t)(x2-x1);
                int32_t dy=(int32_t)(y2-y1);
                int32_t x=(int32_t)x1+dx*(y-y1)/dy;
                //数组越界保护
                if (n<MGL_FILL_POLYGON_MAX_INTERSECTS) {
                    xi[n++]=(mgl_coord_t)x;
                }
            }
        }

        //交点排序
        mgl_sort(xi,n);

        //两两配对画线
        for (mgl_coord_t i=0;i<n-1;i+=2) {
            mgl_ctx_draw_line(ctx,xi[i],y,xi[i+1],y,1,painter);
        }
    }
}