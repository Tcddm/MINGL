#ifndef MGL_DRAW_CTX_TEXT_H
#define MGL_DRAW_CTX_TEXT_H

#include "mgl_draw_ctx.h"
#include "font/mgl_font.h"
#include "core/mgl_math.h"

#ifdef __cplusplus
extern "C"{
#endif

void mgl_ctx_draw_text(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const char *str,const mgl_font_t *font,const mgl_painter_t *painter);
mgl_coord_t mgl_font_get_text_width(const mgl_font_t *font,const char *text);

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_TEXT_H
