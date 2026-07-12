#ifndef MGL_PAGE_MANAGER_H
#define MGL_PAGE_MANAGER_H

#include "mgl_render.h"
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

extern const mgl_page_descriptor_t g_mgl_page_registry[];
extern const uint8_t g_mgl_page_registry_count;

#define MGL_EXPORT_PAGE(name) mgl_page_t *name(void);

#define MGL_PAGE_ENTRY(name,make_func) {(name),(make_func)}

#define MGL_PAGE_WIDGETS_START(name) \
    mgl_page_t *name(){

#define MGL_PAGE_WIDGETS_END() ;}

#define MGL_PAGE_REGISTRY_START() \
    const mgl_page_descriptor_t g_mgl_page_registry[]={

#define MGL_PAGE_REGISTRY_END() \
    {NULL,NULL}}; \
    const uint8_t g_mgl_page_registry_count= \
        sizeof(g_mgl_page_registry)/sizeof(g_mgl_page_registry[0])-1;

bool mgl_page_push(const char *name);
void mgl_page_back(void);
mgl_page_t *mgl_get_current_page(void);
mgl_widget_t *mgl_widget_find_by_id(mgl_widget_t *root,uint16_t id);
mgl_widget_t *mgl_page_find_widget_by_id(mgl_page_t *page,uint16_t id);
mgl_widget_t *mgl_current_page_find_widget_by_id(uint16_t id);

#ifdef __cplusplus
}
#endif
#endif //MGL_PAGE_MANAGER_H
