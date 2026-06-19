#ifndef MGL_DRAW_CTX_BITMAP_H
#define MGL_DRAW_CTX_BITMAP_H

#include "mgl_draw_ctx.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_ctx_draw_bitmap_part
/**
 * @brief 在绘图上下文中绘制位图的部分区域
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标
 * @param src_rect 绘制范围矩形
 * @param bmp 位图指针
 * @param transparent_color 透明色
 * @pre 绘制范围矩形的X坐标和Y坐标要在位图内
 */
void mgl_ctx_draw_bitmap_part(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,const mgl_rect_t *src_rect,const mgl_bitmap_t *bmp,mgl_color_t transparent_color);
// #endregion

// #region mgl_ctx_draw_bitmap
/**
 * @brief 在绘图上下文中绘制位图
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标
 * @param bmp 位图指针
 * @param transparent_color 透明色
 */
static inline void mgl_ctx_draw_bitmap(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,const mgl_bitmap_t *bmp,mgl_color_t transparent_color){
    mgl_rect_t src_rect={0,0,bmp->w,bmp->h};
    mgl_ctx_draw_bitmap_part(ctx,x,y,&src_rect,bmp,transparent_color);
}
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_BITMAP_H
