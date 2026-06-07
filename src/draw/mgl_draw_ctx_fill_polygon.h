
#ifndef MGL_DRAW_FILL_POLYGON_H
#define MGL_DRAW_FILL_POLYGON_H
#include "mgl_draw_ctx.h"
#include "core/mgl_math.h"
#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_ctx_fill_polygon
/**
 * @brief 在绘图上下文中填充多边形
 *
 * @param ctx 绘图上下文指针
 * @param poly 顶点数组（x0，y0，x1，y1，xn，yn）
 * @param cnt 顶点数量
 * @param painter 画笔指针
 * @pre cnt>=3
 * @pre 单条扫描线相交边数量不能超过配置中的MGL_FILL_POLYGON_MAX_INTERSECTS，超出会丢失交点
 */
void mgl_ctx_fill_polygon(mgl_draw_ctx_t *ctx,const mgl_coord_t * poly,uint16_t cnt,const mgl_painter_t *painter);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_FILL_POLYGON_H
