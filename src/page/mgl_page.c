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
void mgl_page_destroy(mgl_page_t *page){
    mgl_page_pool_free(page->pool_start);
}
