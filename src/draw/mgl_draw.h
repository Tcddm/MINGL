#ifndef MGL_DRAW_H
#define MGL_DRAW_H
#include "mgl_draw_ctx.h"
#include "logger/mgl_log.h"
#ifdef __cplusplus
extern "C"{
#endif
static inline void mgl_draw_update_screen_size(mgl_coord_t width,mgl_coord_t height){
    mgl_draw_ctx_t* ctx=&g_mgl_full_screen_ctx;
    ctx->clip.x=0;
    ctx->clip.y=0;
    ctx->clip.w=width;
    ctx->clip.h=height;
    ctx->widget=NULL;
    MGL_LOG_INFO(MGL_LOG_TAG_DRAW,"update screen size: %dx%d",width,height);
}

static inline void mgl_draw_fill_rect(mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,const mgl_painter_t *painter){
    mgl_ctx_fill_rect(&g_mgl_full_screen_ctx, x, y, w, h, painter);
}
#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_H
