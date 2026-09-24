#ifndef MGL_DRAW_CTX_TEXT_H
#define MGL_DRAW_CTX_TEXT_H

#include "mgl_draw_ctx.h"
#include "font/mgl_font.h"
#include "core/mgl_math.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_text_align_t
/**
 * @brief 文本对齐方式（水平与垂直共用）
 */
typedef enum{
    MGL_TEXT_ALIGN_START=0,
    MGL_TEXT_ALIGN_CENTER,
    MGL_TEXT_ALIGN_END
} mgl_text_align_t;
// #endregion

// #region MGL_TEXT_ALIGN_FIELD
/**
 * @brief 文本类控件可复用的对齐字段与处理宏
 * @note 字段默认值为0，即两个方向都是START
 */
#define MGL_TEXT_ALIGN_ARGS_FIELD \
    mgl_text_align_t text_align_h; \
    mgl_text_align_t text_align_v;

#define MGL_TEXT_ALIGN_FIELD \
    mgl_text_align_t text_align_h; \
    mgl_text_align_t text_align_v;

#define MGL_TEXT_ALIGN_FIELD_HANDLE(custom_widget,custom_widget_args) \
    do{ \
        custom_widget->text_align_h=custom_widget_args->text_align_h; \
        custom_widget->text_align_v=custom_widget_args->text_align_v; \
    }while(0)
// #endregion

// #region mgl_ctx_draw_text
/**
 * @brief 在绘图上下文中绘制文本
 *
 * @param ctx 绘图上下文指针
 * @param x X坐标
 * @param y Y坐标（首行基线）
 * @param flags 文本样式（暂不支持）
 * @param str 文本指针（支持\n换行；左对齐、行高为font_size）
 * @param font 字体指针
 * @param painter 画笔指针
 * @note 目前只支持1bpp。需要对齐/行距的多行绘制请使用mgl_ctx_draw_text_box
 */
void mgl_ctx_draw_text(mgl_draw_ctx_t *ctx,mgl_coord_t x,mgl_coord_t y,uint8_t flags,const char *str,const mgl_font_t *font,const mgl_painter_t *painter);
// #endregion

// #region mgl_ctx_draw_text_box
/**
 * @brief 在盒子里绘制多行文本，自动处理换行与对齐
 *
 * @param ctx 绘图上下文指针
 * @param box 目标盒子（决定左/上边界与对齐参考尺寸）
 * @param align_h 水平对齐
 * @param align_v 垂直对齐
 * @param line_spacing 行间距（额外像素，0表示行高仅为font_size）
 * @param flags 文本样式（暂不支持）
 * @param str 文本指针（支持\n换行\r忽略）
 * @param font 字体指针
 * @param painter 画笔指针
 * @note 文本块的偏移会被钳制到0，不会向上/向左溢出盒子
 */
void mgl_ctx_draw_text_box(mgl_draw_ctx_t *ctx,const mgl_rect_t *box,
                           mgl_text_align_t align_h,mgl_text_align_t align_v,
                           mgl_coord_t line_spacing,uint8_t flags,
                           const char *str,const mgl_font_t *font,
                           const mgl_painter_t *painter);
// #endregion

// #region mgl_font_get_text_size
/**
 * @brief 获取绘制文本所需尺寸
 *
 * @param font 字体指针
 * @param text 文本指针（支持 \n 换行，\r 忽略）
 * @param line_spacing 行间距（额外像素）
 * @param[out] out_w 宽度（各行最大宽度）
 * @param[out] out_h 高度（首行为 font_size，其余每行为 font_size+line_spacing）
 */
void mgl_font_get_text_size(const mgl_font_t *font,const char *text,
                            mgl_coord_t line_spacing,
                            mgl_coord_t *out_w,mgl_coord_t *out_h);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_DRAW_CTX_TEXT_H
