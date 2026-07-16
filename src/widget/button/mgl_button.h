#ifndef MGL_BUTTON_H
#define MGL_BUTTON_H

#include "widget/mgl_widget.h"
#include "font/mgl_font.h"
#include "draw/mgl_draw_ctx_round_rect.h"
#include "draw/mgl_draw_ctx_text.h"
#include "draw/mgl_draw_ctx_bitmap.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef enum{
    MGL_BUTTON_STATE_NORMAL=0,
    MGL_BUTTON_STATE_PRESSED,
    MGL_BUTTON_STATE_DISABLE
} mgl_button_state_t;

void *mgl_button_init(void *memory,const void *args);

typedef struct {
    const char *text;
    const mgl_font_t *font;
    const mgl_bitmap_t *bmp;
    uint8_t spacing;
    uint8_t padding;
    MGL_WIDGET_BASE_ARGS_FIELD
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(foreground)
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(background)
    MGL_WIDGET_ROUND_RADIUS_FIELD
} mgl_button_args_t;

typedef struct {
    mgl_widget_t base;
    const char *text;
    const mgl_font_t *font;
    const mgl_bitmap_t *bmp;
    uint8_t spacing;
    uint8_t padding;
    uint8_t state;
    MGL_WIDGET_PAINTER_FIELD_NAME(foreground)
    MGL_WIDGET_PAINTER_FIELD_NAME(background)
    MGL_WIDGET_ROUND_RADIUS_FIELD
} mgl_button_t;

#define MGL_BUTTON(...) \
    &((mgl_widget_blueprint_t){ \
        .init=mgl_button_init, \
        .size=sizeof(mgl_button_t), \
        .args=&(mgl_button_args_t){ __VA_ARGS__ } \
    })

#ifdef __cplusplus
}
#endif
#endif //MGL_BUTTON_H
