#ifndef MGL_SLIDER_H
#define MGL_SLIDER_H

#include "widget/mgl_widget.h"
#include "draw/mgl_draw_ctx_round_rect.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef MGL_SLIDER_DEFAULT_MAX
#define MGL_SLIDER_DEFAULT_MAX 100
#endif
#ifndef MGL_SLIDER_DEFAULT_TRACK_H
#define MGL_SLIDER_DEFAULT_TRACK_H 4
#endif
#ifndef MGL_SLIDER_DEFAULT_THUMB_W
#define MGL_SLIDER_DEFAULT_THUMB_W 24
#endif
#ifndef MGL_SLIDER_DEFAULT_THUMB_H
#define MGL_SLIDER_DEFAULT_THUMB_H 24
#endif

void *mgl_slider_init(void *memory,const void *args);

typedef struct {
    int16_t value;
    int16_t min;
    int16_t max;
    uint8_t track_h;
    uint8_t thumb_w;
    uint8_t thumb_h;
    MGL_WIDGET_BASE_ARGS_FIELD
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(track)
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(fill)
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(thumb)
    MGL_WIDGET_ROUND_RADIUS_FIELD
} mgl_slider_args_t;

typedef struct{
    mgl_widget_t base;
    int16_t value;
    int16_t min,max;
    uint8_t track_h,thumb_w,thumb_h;
    bool dragging;
    MGL_WIDGET_PAINTER_FIELD_NAME(track)
    MGL_WIDGET_PAINTER_FIELD_NAME(fill)
    MGL_WIDGET_PAINTER_FIELD_NAME(thumb)
    MGL_WIDGET_ROUND_RADIUS_FIELD
} mgl_slider_t;

#define MGL_SLIDER(...) \
    &((mgl_widget_blueprint_t){ \
        .init=mgl_slider_init, \
        .size=sizeof(mgl_slider_t), \
        .args=&(mgl_slider_args_t){__VA_ARGS__} \
    })

#ifdef __cplusplus
}
#endif
#endif //MGL_SLIDER_H
