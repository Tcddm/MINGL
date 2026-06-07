
#ifndef MGL_DRAW_CTX_ARC_H
#define MGL_DRAW_CTX_ARC_H

#include "mgl_draw_ctx_fill_polygon.h"


#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_ctx_draw_arc
/**
 * @brief 在绘图上下文中绘制环形圆弧
 *
 * @param ctx 绘图上下文指针
 * @param cx X坐标
 * @param cy Y坐标
 * @param radius 半径
 * @param w 边框宽度
 * @param start_angle 起始角（0为正右方）
 * @param end_angle 终止角（逆时针递增）
 * @param painter 画笔指针
 * @pre radius>0,w>0,radius>w
 */
void mgl_ctx_draw_arc(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,mgl_coord_t w,uint16_t start_angle,uint16_t end_angle,const mgl_painter_t *painter);
// #endregion

// #region mgl_ctx_fill_arc
/**
 * @brief 在绘图上下文中绘制实心扇形圆弧
 *
 * @param ctx 绘图上下文指针
 * @param cx X坐标
 * @param cy Y坐标
 * @param radius 半径
 * @param start_angle 起始角（0为正右方）
 * @param end_angle 终止角（逆时针递增）
 * @param painter 画笔指针
 * @pre radius>0
 */
void mgl_ctx_fill_arc(mgl_draw_ctx_t *ctx,mgl_coord_t cx,mgl_coord_t cy,mgl_coord_t radius,uint16_t start_angle,uint16_t end_angle,const mgl_painter_t *painter);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_ARC_H
