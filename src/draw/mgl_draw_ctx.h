
#ifndef MGL_DRAW_CTX_H
#define MGL_DRAW_CTX_H

#include "hal/mgl_hal.h"
#include "painter/mgl_painter.h"
#include "core/mgl_static_assert.h"
#include "core/mgl_math.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region g_mgl_full_screen_ctx
/**
 * @brief 全屏绘图上下文
 * @note clip矩形的宽度高度是屏幕的宽度高度，控件指针为NULL
 */
extern mgl_draw_ctx_t g_mgl_full_screen_ctx;
// #endregion

// #region mgl_ctx_init
/**
 * @brief 初始化
 *
 * @param ctx  待初始化的绘图上下文指针
 * @param widget`关联的控件
 * @param clip 初始裁剪区域
 */
void mgl_ctx_init(mgl_draw_ctx_t *ctx,mgl_widget_t *widget,const mgl_rect_t *clip);
// #endregion

// #region mgl_ctx_draw_pixel
/**
 * @brief 在绘图上下文中画点
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标
 * @param painter 画笔指针
 */
static inline void mgl_ctx_draw_pixel(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,const mgl_painter_t *painter){
    const mgl_rect_t *r=&ctx->clip;
    if (x>=r->x&&x<r->x+r->w&&
        y>=r->y&&y<r->y+r->h){
        if(painter->type==MGL_PAINTER_TYPE_SOLID){
            mgl_hal_set_pixel(x,y,painter->solid_color);
        }else{
            painter->vtable->set_pixel(painter,x,y);
        }
    }
}
// #endregion

// #region mgl_ctx_draw_rect
/**
 * @brief 在绘图上下文中绘制边框矩形
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标
 * @param w 宽度
 * @param h 高度
 * @param width 边框宽度
 * @param painter 画笔指针
 * @pre w>=width*2,h>=width*2
 */
void mgl_ctx_draw_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_coord_t width,const mgl_painter_t *painter);
// #endregion

// #region mgl_ctx_fill_rect
/**
 * @brief 在绘图上下文中绘制实心矩形
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标
 * @param w 宽度
 * @param h 高度
 * @param painter 画笔指针
 */
void mgl_ctx_fill_rect(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,const mgl_painter_t *painter);
// #endregion

// #region mgl_ctx_draw_line
/**
 * @brief 在绘图上下文中绘制直线
 *
 * @param ctx 绘图上下文指针
 * @param x0 第一个点X坐标
 * @param y0 第一个点Y坐标
 * @param x1 第二个点X坐标
 * @param y1 第二个点Y坐标
 * @param w 直线宽度
 * @param painter 画笔指针
 * @pre w>0
 */
void mgl_ctx_draw_line(mgl_draw_ctx_t *ctx,mgl_coord_t x0,mgl_coord_t y0,mgl_coord_t x1,mgl_coord_t y1,mgl_coord_t w,const mgl_painter_t *painter);
// #endregion

// #region mgl_rect_intersect
/**
 * @brief 计算两个矩形的相交区域
 *
 * @param a 矩形A
 * @param b 矩形B
 * @param[out] out 相交结果矩形
 * @return  是否有交集
 */
bool mgl_rect_intersect(const mgl_rect_t *a,const mgl_rect_t *b,mgl_rect_t *out);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_H
