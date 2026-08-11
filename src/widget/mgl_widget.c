#include "mgl_widget.h"
void mgl_widget_init(mgl_widget_t *widget,const mgl_widget_vtable_t *vtable,const char *name,void *user_data){
    widget->vtable=vtable;
    widget->id=0;
    
    widget->bounds=(mgl_rect_t){0, 0, 0, 0};
    widget->prev_bounds=(mgl_rect_t){0, 0, 0, 0};
    widget->margin=(mgl_margin_t){0,0,0,0};
    
    widget->parent=NULL;
    widget->first_child=NULL;
    widget->next_sibling=NULL;

    widget->action_handler=NULL;
    
    widget->dirty=1;
    widget->layout_dirty=0;
    widget->hidden=0;
    widget->enabled=1;
    widget->focused=0;
    widget->reserved=0;

    widget->anim_slot=-1;

    widget->user_data=user_data;
#if MGL_WIDGET_NAME_FIELD_ENABLE
    widget->name=name;
#else
    (void)name;
#endif



}

void mgl_widget_add_child(mgl_widget_t *parent,mgl_widget_t *child){
    if(!child){return;}
    child->parent=parent;
    child->next_sibling=NULL;

    if(!parent->first_child){
        parent->first_child=child;
    }else{
        mgl_widget_t *last=parent->first_child;
        while(last->next_sibling){
            last=last->next_sibling;
        }
        last->next_sibling=child;
    }
}
