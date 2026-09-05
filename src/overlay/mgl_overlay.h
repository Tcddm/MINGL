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
void mgl_overlay_show(mgl_widget_t *w);
void mgl_overlay_hide(mgl_widget_t *w);

#else

static inline mgl_page_t *mgl_page_get_overlay(void){ return NULL;}
static inline void mgl_page_set_overlay(mgl_page_t *page){ (void)page;}
static inline mgl_widget_t *mgl_overlay_find_widget_by_id(uint16_t id){
    (void)id;
    return NULL;
}
static inline void mgl_overlay_show(mgl_widget_t *w){(void)w;}
static inline void mgl_overlay_hide(mgl_widget_t *w){(void)w;}

#endif

void mgl_toast_show(const char *str,uint32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif //MGL_OVERLAY_H
