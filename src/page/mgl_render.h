#ifndef MGL_RENDER_H
#define MGL_RENDER_H

#include "core/mgl_rect.h"
#include "mgl_page.h"

#ifdef __cplusplus
extern "C"{
#endif

// #region mgl_render_page
/**
 * @brief 渲染页面
 *
 * @param page 页面指针
 * @param screen 屏幕区域矩形（通常为全屏），用于初始化裁剪区域
 */
void mgl_render_page(mgl_page_t *page,mgl_rect_t screen);
// #endregion

// #region mgl_render_widget
/**
 * 渲染控件树
 *
 * @param root 根组件指针
 * @param screen_clip 屏幕区域矩形（通常为全屏），用于初始化裁剪区域
 * @param[out] flush_rects 视图刷新的矩形数组
 * @param[out] flush_count 视图刷新的矩形数量（0表示需要全屏幕刷新）
 */
void mgl_render_widget(mgl_widget_t *root,const mgl_rect_t *screen_clip,mgl_rect_t *flush_rects,uint8_t *flush_count);
// #endregion

#ifdef __cplusplus
}
#endif
#endif //MGL_RENDER_H
