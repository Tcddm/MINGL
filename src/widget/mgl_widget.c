#include "mgl_widget.h"
void mgl_widget_init(mgl_widget_t *widget,const mgl_widget_vtable_t *vtable,const char *name,void *user_data,uint16_t type){
    widget->vtable=vtable;
    widget->type=type;
    widget->id=0;
    
    widget->bounds=(mgl_rect_t){0, 0, 0, 0};
    widget->margin=(mgl_margin_t){0,0,0,0};
    
    widget->parent=NULL;
    widget->first_child=NULL;
    widget->next_sibling=NULL;
    
    widget->dirty=1;
    widget->visible=1;
    widget->enabled=1;
    widget->focused=0;
    widget->reserved=0;

    widget->anim_slot=-1;

    widget->user_data=user_data;
    widget->name=name;



}

void mgl_widget_add_child(mgl_widget_t *parent, mgl_widget_t *child) {
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

mgl_action_type_t mgl_widget_get_action(mgl_widget_t *self,const mgl_event_t *event){
    if (event->type==MGL_EVENT_TOUCH_UP) {
        return MGL_ACTION_CLICK;
    }
    return MGL_ACTION_NONE;
}
