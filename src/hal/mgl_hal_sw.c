#include "mgl_hal.h"
#if !MGL_HW_ACCEL_FILL_RECT
void mgl_hal_fill_rect(mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_color_t color){
    const mgl_coord_t x_end=(mgl_coord_t)(x+w);
    const mgl_coord_t y_end=(mgl_coord_t)(y+h);

    for (mgl_coord_t dy=y;dy<y_end;dy++){
        for (mgl_coord_t dx=x;dx<x_end;dx++){
            mgl_hal_set_pixel(dx,dy,color);
        }
    }
}
#endif

#if !MGL_HW_ACCEL_BIT_BLT
void mgl_hal_bit_blt(mgl_coord_t x,mgl_coord_t y,const mgl_rect_t *src_rect,const mgl_bitmap_t *bmp,mgl_color_t transparent_color){
    mgl_coord_t src_x,src_y;
    mgl_coord_t src_w,src_h,bmp_w;
    src_x=src_rect->x;
    src_y=src_rect->y;
    src_w=src_rect->w;
    src_h=src_rect->h;
    bmp_w=bmp->w;
    switch (bmp->format) {
        case MGL_BITMAP_FORMAT_RGB565:{
            const uint16_t *pixels=(const uint16_t *)bmp->data;
            for (mgl_coord_t dy=0;dy<src_h;dy++) {
                for (mgl_coord_t dx=0;dx<src_w;dx++) {
                    uint32_t pixel_idx=(src_y+dy)*bmp_w+(src_x+dx);
                    mgl_color_t color={.value=pixels[pixel_idx],.alpha=255};
                    if(color.value!=transparent_color.value){
                        mgl_hal_set_pixel((mgl_coord_t)(x+dx),(mgl_coord_t)(y+dy),color);
                    }

                }
            }
            }
            break;
        default:
            return;
    }

}
#endif