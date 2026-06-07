#ifndef MGL_DRAW_CTX_ROUND_RECT_H
#define MGL_DRAW_CTX_ROUND_RECT_H

#include "mgl_draw_ctx_arc.h"

#ifdef __cplusplus
extern "C"{
#endif

void mgl_ctx_fill_round_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_coord_t radius,const mgl_painter_t *painter);

void mgl_ctx_draw_round_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_coord_t radius,mgl_coord_t width,const mgl_painter_t *painter);

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_ROUND_RECT_H
