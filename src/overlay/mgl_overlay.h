#ifndef MGL_OVERLAY_H
#define MGL_OVERLAY_H

#include "mgl_config.h"
#include "page/mgl_page.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef enum {
    MGL_OVERLAY_TOAST=1001,
} mgl_overlay_id_t;

#if MGL_OVERLAY

mgl_page_t *mgl_page_get_overlay(void);
void mgl_page_set_overlay(mgl_page_t *page);
mgl_widget_t *mgl_overlay_find_widget_by_id(uint16_t id);

#else

static inline mgl_page_t *mgl_page_get_overlay(void){ return NULL;}
static inline void mgl_page_set_overlay(mgl_page_t *page){ (void)page;}
static inline mgl_widget_t *mgl_overlay_find_widget_by_id(uint16_t id){
    (void)id;
    return NULL;
}

#endif

#ifdef __cplusplus
}
#endif

#endif //MGL_OVERLAY_H
