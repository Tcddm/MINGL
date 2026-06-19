#ifndef MGL_DRAW_CTX_TEXT_H
#define MGL_DRAW_CTX_TEXT_H

#include "mgl_draw_ctx.h"
#include "font/mgl_font.h"
#include "core/mgl_math.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_ctx_draw_text
/**
 * @brief 在绘图上下文中绘制文本
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标
 * @param flags 文本样式（暂不支持）
 * @param str 文本指针
 * @param font 字体指针
 * @param painter 画笔指针
 * @note 目前只支持1bpp
 */
void mgl_ctx_draw_text(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const char *str,const mgl_font_t *font,const mgl_painter_t *painter);
// #endregion

// #region mgl_font_get_text_width
/**
 * @brief 获取绘制文本所需宽度
 *
 * @param font 字体指针
 * @param text 文本指针
 * @return 绘制文本所需宽度
 */
mgl_coord_t mgl_font_get_text_width(const mgl_font_t *font,const char *text);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_TEXT_H
