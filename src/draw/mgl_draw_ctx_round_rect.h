#ifndef MGL_DRAW_CTX_ROUND_RECT_H
#define MGL_DRAW_CTX_ROUND_RECT_H

#include "mgl_draw_ctx_arc.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_ctx_fill_round_rect
/**
 * @brief 在绘图上下文中绘制实心圆角矩形
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标
 * @param w 宽度
 * @param h 高度
 * @param radius 圆角半径
 * @param painter 画笔指针
 * @pre w>=2*radius,h>=2*radius,radius>=0
 * @note 圆角半径为0时退化为矩形
 */
void mgl_ctx_fill_round_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_coord_t radius,const mgl_painter_t *painter);
// #endregion

// #region mgl_ctx_draw_round_rect
/**
 * @brief 在绘图上下文中绘制边框圆角矩形
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标
 * @param w 宽度
 * @param h 高度
 * @param radius 圆角半径
 * @param width 边框宽度
 * @param painter 画笔指针
 * @pre w>=2*radius,h>=2*radius,w>=2*width,h>=2*width,
 * radius>=0,width>0
 * @note 圆角半径为0时退化为矩形
 */
void mgl_ctx_draw_round_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_coord_t radius,mgl_coord_t width,const mgl_painter_t *painter);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_ROUND_RECT_H
