#include "mgl_overlay.h"
#include "page/mgl_page_manager.h"

static mgl_page_t *g_overlay_page;

void mgl_page_set_overlay(mgl_page_t *page){
    g_overlay_page=page;
}

mgl_page_t *mgl_page_get_overlay(void){
    return g_overlay_page;
}

mgl_widget_t *mgl_overlay_find_widget_by_id(uint16_t id){
    if(!g_overlay_page||!g_overlay_page->root){return NULL;}
    return mgl_widget_find_by_id(g_overlay_page->root,id);
}

