#include "mgl_fb.h"
#include "string.h"
#if MGL_FRAMEBUFFER_INTERNAL
static mgl_color_value_t fb_buf[MGL_SCREEN_WIDTH*MGL_SCREEN_HEIGHT];

static mgl_fb_t g_fb={
        .data=fb_buf,
        .width=MGL_SCREEN_WIDTH,
        .height=MGL_SCREEN_HEIGHT,
        .stride_px=MGL_SCREEN_WIDTH
};
#else
static mgl_fb_t g_fb;
#endif

void mgl_hal_set_fb(const mgl_fb_t *fb){
    g_fb=*fb;
}
const mgl_color_value_t *mgl_hal_get_fb(void){
    return g_fb.data;
}

void mgl_hal_set_pixel(mgl_coord_t x,mgl_coord_t y,mgl_color_value_t color){
    g_fb.data[(uint32_t)y*g_fb.stride_px+x]=color;
}

void mgl_hal_fill_rect(mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_color_value_t color){
    for(int row=y;row<y+h;row++){
        mgl_color_value_t *line=&g_fb.data[(uint32_t)row*g_fb.stride_px+x];
        for(int col=0;col<w;col++){
            line[col]=color;
        }
    }
}

void mgl_hal_clear_screen(void){
    memset(g_fb.data,0,g_fb.stride_px*g_fb.height*sizeof(mgl_color_value_t));
}

void mgl_hal_bit_blt(mgl_coord_t x,mgl_coord_t y,
                     const mgl_rect_t *src_rect,
                     const mgl_bitmap_t *bmp,
                     mgl_color_value_t transparent_color){
    if(bmp->format!=MGL_BITMAP_FORMAT_RGB565){return;}

    mgl_color_value_t *buf=(mgl_color_value_t *)g_fb.data;
    const mgl_color_value_t *pixels=(const mgl_color_value_t *)bmp->data;
    mgl_coord_t src_x=src_rect->x,src_y=src_rect->y;
    mgl_coord_t src_w=src_rect->w,src_h=src_rect->h;
    mgl_coord_t bmp_w=bmp->w;

    for(mgl_coord_t dy=0;dy<src_h;dy++){
        mgl_color_value_t *dst=&buf[(uint32_t)(y+dy)*g_fb.stride_px+x];
        const mgl_color_value_t *src=&pixels[(src_y+dy)*bmp_w+src_x];
        for(mgl_coord_t dx=0;dx<src_w;dx++){
            if(src[dx]!=transparent_color){
                dst[dx]=src[dx];
            }
        }
    }
}

static inline mgl_color_value_t mgl_rgb565_blend(mgl_color_value_t fg,uint8_t alpha,mgl_color_value_t bg){
    uint8_t inv=255-alpha;

    uint8_t fr=(fg >> 11) & 0x1F,fg_g=(fg >> 5) & 0x3F,fg_b=fg & 0x1F;
    uint8_t br=(bg >> 11) & 0x1F,bg_g=(bg >> 5) & 0x3F,bg_b=bg & 0x1F;

    uint8_t r=(fr*alpha+br*inv)/255;
    uint8_t g=(fg_g*alpha+bg_g*inv)/255;
    uint8_t b=(fg_b*alpha+bg_b*inv)/255;

    return (mgl_color_value_t)((r << 11) | (g << 5) | b);
}
#if !MGL_HW_ACCEL_ALPHA_BLEND
void mgl_hal_fill_rect_alpha(mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_color_t color){
    uint8_t alpha=color.alpha;
    if(alpha==255){
        mgl_hal_fill_rect(x,y,w,h,color.value);
        return;
    }
    if(alpha==0){ return;}

    for(mgl_coord_t row=y;row<y+h;row++){
        mgl_color_value_t *line=&g_fb.data[(uint32_t)row*g_fb.stride_px+x];
        for(mgl_coord_t col=0;col<w;col++){
            line[col]=mgl_rgb565_blend(color.value,alpha,line[col]);
        }
    }
}
void mgl_hal_bit_blt_alpha(mgl_coord_t x,mgl_coord_t y,const mgl_rect_t *src_rect,const mgl_bitmap_t *bmp,mgl_color_value_t transparent_color,uint8_t alpha){
    if(bmp->format!=MGL_BITMAP_FORMAT_RGB565){ return;}
    if(alpha==0){ return;}

    const mgl_color_value_t *pixels=(const mgl_color_value_t *)bmp->data;
    mgl_coord_t sx=src_rect->x,sy=src_rect->y;
    mgl_coord_t sw=src_rect->w,sh=src_rect->h;

    bool opaque=(alpha==255);

    for(mgl_coord_t dy=0;dy<sh;dy++){
        mgl_color_value_t *dst=&g_fb.data[(uint32_t)(y+dy)*g_fb.stride_px+x];
        const mgl_color_value_t *src=&pixels[(sy+dy)*bmp->w+sx];
        for(mgl_coord_t dx=0;dx<sw;dx++){
            if(src[dx]!=transparent_color){
                dst[dx]=opaque ? src[dx]
                        : mgl_rgb565_blend(src[dx],alpha,dst[dx]);
            }
        }
    }
}
#endif