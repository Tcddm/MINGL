#ifndef MGL_LINEAR_LAYOUT_H
#define MGL_LINEAR_LAYOUT_H

#include "widget/mgl_layout_utils.h"
#include "widget/mgl_widget_blueprint.h"
#include "draw/mgl_draw_ctx_round_rect.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    mgl_widget_t base;
    uint8_t direction;
    mgl_margin_t padding_margin;
    MGL_WIDGET_ACTION_HANDLER
    MGL_WIDGET_PAINTER_FIELD
    MGL_WIDGET_ROUND_RADIUS_FIELD
    uint8_t cross_align;
} mgl_linear_layout_t;

typedef struct {
    mgl_margin_t padding_margin;
    uint8_t direction;
    uint8_t cross_align;
    MGL_WIDGET_ACTION_HANDLER
    MGL_WIDGET_BASE_ARGS_FIELD
    MGL_WIDGET_PAINTER_ARGS_FIELD
    MGL_WIDGET_ROUND_RADIUS_FIELD
    const mgl_widget_blueprint_t **children;
} mgl_linear_layout_args_t;

void *mgl_linear_layout_init(void *memory,const void *args);


#define MGL_CHILDREN(...)  (const mgl_widget_blueprint_t *[]){ __VA_ARGS__, NULL }

#define MGL_LINEAR_LAYOUT(...) \
    &((mgl_widget_blueprint_t){ \
        .init = mgl_linear_layout_init, \
        .size = sizeof(mgl_linear_layout_t), \
        .args = &(mgl_linear_layout_args_t){ __VA_ARGS__ } \
    })

#ifdef __cplusplus
}
#endif
#endif //MGL_LINEAR_LAYOUT_H
