
#ifndef MGL_DRAW_CTX_CIRCLE_H
#define MGL_DRAW_CTX_CIRCLE_H

#include "mgl_draw_ctx_arc.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_ctx_draw_circle
/**
 * @brief 在绘图上下文中绘制带宽度空心圆
 *
 * @param ctx 绘图上下文指针
 * @param cx 圆心X坐标
 * @param cy 圆心Y坐标
 * @param radius 半径
 * @param w 宽度
 * @param painter 画笔指针
 * @note 宽度为1px时使用Bresenham中点画圆算法
 * @pre radius>0,w>0
 */
void mgl_ctx_draw_circle(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,mgl_coord_t w,const mgl_painter_t *painter);
// #endregion

// #region mgl_ctx_fill_circle
/**
 * @brief 在绘图上下文中绘制实心圆
 *
 * @param ctx 绘图上下文指针
 * @param cx 圆心X坐标
 * @param cy 圆心Y坐标
 * @param radius 半径
 * @param quick 是否使用快速绘制
 * @param painter 画笔指针
 * @note 快速绘制为使用Bresenham中点画圆算法，否则为画实心扇形圆弧
 * @pre radius>0
 */
void mgl_ctx_fill_circle(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,bool quick,const mgl_painter_t *painter);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_CIRCLE_H
