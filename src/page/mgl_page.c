#include "mgl_page.h"

mgl_page_t *mgl_page_create_from_blueprint(const mgl_widget_blueprint_t *root_bp){
    uint8_t *start=mgl_page_pool_get_top();

    mgl_widget_t *root=mgl_blueprint_collect(root_bp,NULL);
    if (!root) {
        mgl_page_pool_free(start);
        return NULL;
    }
    mgl_page_t *page=(mgl_page_t *)mgl_page_pool_alloc(sizeof(mgl_page_t));
    if (!page) {
        mgl_page_pool_free(start);
        return NULL;
    }

    page->root = root;
    page->pool_start = start;
    return page;
}
void mgl_page_mark_all_widget_dirty(mgl_widget_t *root){
    if (!root) return;
    mgl_widget_t *w = root;
    while (w) {
        w->dirty = true;
        if (w->first_child) {
            w = w->first_child;
        } else {
            while (w && !w->next_sibling) {
                w = w->parent;
            }
            if (w) w = w->next_sibling;
        }
    }
}
