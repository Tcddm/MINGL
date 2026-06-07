#ifndef MGL_LABEL_H
#define MGL_LABEL_H

#include "widget/mgl_widget.h"
#include "draw/mgl_draw_ctx_text.h"

#ifdef __cplusplus
extern "C"{
#endif

void *mgl_label_init(void *memory,const void *args);

typedef struct {
    const char *text;
    const mgl_font_t *font;
    MGL_WIDGET_BASE_ARGS_FIELD
    MGL_WIDGET_PAINTER_ARGS_FIELD
} mgl_label_args_t;

typedef struct {
    mgl_widget_t base;
    const char *text;
    const mgl_font_t *font;
    MGL_WIDGET_PAINTER_FIELD
} mgl_label_t;

#define MGL_TEXT(...) \
    &((mgl_widget_blueprint_t){ \
        .init=mgl_label_init, \
        .size=sizeof(mgl_label_t), \
        .args=&(mgl_label_args_t){ __VA_ARGS__ } \
    })

#ifdef __cplusplus
}
#endif
#endif //MGL_LABEL_H
