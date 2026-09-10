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

void mgl_overlay_show(mgl_widget_t *w){
    if(!w){ return;}
    w->hidden=0;
    mgl_widget_set_dirty(w);
    mgl_page_t *page=mgl_get_current_page();
    if(page && page->root){
        mgl_widget_set_dirty_full(page->root);
    }
}

void mgl_overlay_hide(mgl_widget_t *w){
    if(!w){ return;}
    w->hidden=1;
    mgl_widget_set_dirty(w);
    mgl_page_t *page=mgl_get_current_page();
    if(page && page->root){
        mgl_widget_set_dirty_full(page->root);
    }
}