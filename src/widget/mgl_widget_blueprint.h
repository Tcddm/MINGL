#ifndef MGL_WIDGET_BLUEPRINT_H
#define MGL_WIDGET_BLUEPRINT_H

#include "mgl_widget.h"
#include "pool/mgl_page_pool.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    void *(*init)(void *memory,const void *args);
    uint16_t size;
    const void *args;
} mgl_widget_blueprint_t;

mgl_widget_t *mgl_blueprint_collect(const mgl_widget_blueprint_t *bp,mgl_widget_t *parent);

#ifdef __cplusplus
}
#endif
#endif //MGL_WIDGET_BLUEPRINT_H
