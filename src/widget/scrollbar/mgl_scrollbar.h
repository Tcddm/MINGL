#ifndef MGL_SCROLLBAR_H
#define MGL_SCROLLBAR_H

#include "widget/mgl_widget.h"
#include "draw/mgl_draw_ctx_round_rect.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef MGL_SCROLLBAR_DEFAULT_BAR_W
#define MGL_SCROLLBAR_DEFAULT_BAR_W 15
#endif
#ifndef MGL_SCROLLBAR_DEFAULT_THUMB_MIN_H
#define MGL_SCROLLBAR_DEFAULT_THUMB_MIN_H 20
#endif

// #region mgl_scrollbar_callback_t
typedef void (*mgl_scrollbar_callback_t)(int32_t position,void *user_data);
// #endregion

typedef struct{
    uint8_t bar_w;
    mgl_scrollbar_callback_t on_scroll;
    void *callback_data;
    MGL_WIDGET_BASE_ARGS_FIELD
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(track)
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(thumb)
} mgl_scrollbar_args_t;

typedef struct {
    mgl_widget_t base;
    int32_t content_total;
    int32_t viewport_h;
    int32_t position;
    uint8_t bar_w;
    bool dragging;
    int16_t touch_start_y;
    int32_t position_at_start;
    mgl_scrollbar_callback_t on_scroll;
    void *callback_data;
    MGL_WIDGET_PAINTER_FIELD_NAME(track)
    MGL_WIDGET_PAINTER_FIELD_NAME(thumb)
} mgl_scrollbar_t;

void *mgl_scrollbar_init(void *memory,const void *args);

/**
 * @brief 设置内容总高度、可见区域高度和当前滚动位置
 *
 * @param sb 滚动条
 * @param content_total 内容总高度
 * @param viewport_h 可见区域高度
 * @param position 当前滚动位置
 */
void mgl_scrollbar_set_content(mgl_scrollbar_t *sb,
                               int32_t content_total,int32_t viewport_h,
                               int32_t position);

#define MGL_SCROLLBAR(...) \
    &((mgl_widget_blueprint_t){ \
        .init=mgl_scrollbar_init, \
        .size=sizeof(mgl_scrollbar_t), \
        .args=&(mgl_scrollbar_args_t){ __VA_ARGS__ } \
    })

#ifdef __cplusplus
}
#endif
#endif //MGL_SCROLLBAR_H
