#ifndef MGL_PAINTER_H
#define MGL_PAINTER_H

#include "mgl_color.h"
#include "core/mgl_types.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct mgl_painter_t mgl_painter_t;

typedef struct {
    void (*fill_rect)(const mgl_painter_t *self,mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h);
    void (*set_pixel)(const mgl_painter_t *self,mgl_coord_t x,mgl_coord_t y);
} mgl_painter_vtable_t;

typedef enum{
    MGL_PAINTER_TYPE_EMPTY=0,
    MGL_PAINTER_TYPE_SOLID,
    MGL_PAINTER_TYPE_CUSTOM
} mgl_painter_type_t;

struct mgl_painter_t {
    uint8_t type;
    union {
        mgl_color_t solid_color;
        const mgl_painter_vtable_t *vtable;
    };
};

extern const mgl_painter_vtable_t g_mgl_empty_painter_vtable;

#define MGL_WIDGET_PAINTER_FIELD_NAME(name) \
    mgl_painter_t name;

#define MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(name) \
    const mgl_painter_t *name;

#define MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(custom_widget,custom_widget_args,name) \
    do{ \
        if(custom_widget_args->name){ \
            custom_widget->name=*(custom_widget_args->name); \
        }else{ \
            custom_widget->name.type=MGL_PAINTER_TYPE_EMPTY; \
            custom_widget->name.vtable=&g_mgl_empty_painter_vtable; \
        } \
    }while(0)

#define MGL_WIDGET_PAINTER_FIELD \
    MGL_WIDGET_PAINTER_FIELD_NAME(painter)

#define MGL_WIDGET_PAINTER_ARGS_FIELD \
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(painter)

#define MGL_WIDGET_PAINTER_FIELD_HANDLE(custom_widget,custom_widget_args) \
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(custom_widget,custom_widget_args,painter)

#ifdef __cplusplus
}
#endif
#endif //MGL_PAINTER_H
