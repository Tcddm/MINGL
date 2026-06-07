#include "mgl_widget_blueprint.h"
mgl_widget_t *mgl_blueprint_collect(const mgl_widget_blueprint_t *bp,mgl_widget_t *parent){
    void *memory=mgl_page_pool_alloc(bp->size);
    if(!memory){return NULL;}
    mgl_widget_t *widget=bp->init(memory,bp->args);
    if(!widget){return NULL;}
    if(parent){
        mgl_widget_add_child(parent,widget);
    }
    return widget;
}