#ifndef MGL_PAGE_H
#define MGL_PAGE_H

#include "draw/mgl_draw_ctx.h"
#include "widget/mgl_widget_blueprint.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct mgl_page_t mgl_page_t;

typedef struct {
    const char *name;
    mgl_page_t *(*make)(void);
} mgl_page_descriptor_t;

struct mgl_page_t{
    const mgl_page_descriptor_t *desc;
    mgl_widget_t *root;
    uint8_t *pool_start;
};


mgl_page_t *mgl_page_create_from_blueprint(const mgl_widget_blueprint_t *root_bp);
void mgl_page_mark_all_widget_dirty(mgl_widget_t *root);

#define MGL_ROOT(root_widget) \
    return mgl_page_create_from_blueprint(root_widget)

#ifdef __cplusplus
}
#endif
#endif //MGL_PAGE_H
