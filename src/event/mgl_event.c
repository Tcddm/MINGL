#include "mgl_event.h"
static mgl_widget_t *get_deepest_hit_widget(mgl_widget_t *root,mgl_coord_t x,mgl_coord_t y){
    if(!root||!root->visible){return NULL;}

    mgl_widget_t *stack[MGL_MAX_WIDGET_DEPTH];
    int stack_top=0;
    stack[stack_top++]=root;
    mgl_widget_t *hit=NULL;

    while(stack_top>0){
        mgl_widget_t *w=stack[--stack_top];
        if(!w->visible||!w->enabled){continue;}

        if(x>=w->bounds.x&&x<w->bounds.x+w->bounds.w&&
            y>=w->bounds.y&&y<w->bounds.y+w->bounds.h){
            hit=w;

            mgl_widget_t *child=w->first_child;
            while(child){
                if(stack_top<MGL_MAX_WIDGET_DEPTH){
                    stack[stack_top++] = child;
                }
                child=child->next_sibling;
            }
        }
    }
    return hit;
}

static bool event_bubble(mgl_widget_t *from,mgl_event_t *event){
    mgl_widget_t *current=from;
    while(current){
        if(current->vtable&&current->vtable->on_event){
            if(current->vtable->on_event(current, event)){
                return true;
            }
        }
        current=current->parent;
    }
    return false;
}

bool mgl_dispatch_touch_event(mgl_widget_t *root,mgl_event_t *event){
    mgl_widget_t *target= get_deepest_hit_widget(root,event->touch.x,event->touch.y);
    if(!target){return false;}
    return event_bubble(target, event);
}

static bool g_touch_pressed=false;
static mgl_coord_t g_last_x,g_last_y;

void mgl_process_touch_data(const mgl_touch_data_t *data,mgl_widget_t *root){
    if(data&&data->count>0){
        if(!g_touch_pressed){
            g_last_x=data->points[0].x;
            g_last_y=data->points[0].y;
            mgl_event_t event={
                    .type=MGL_EVENT_TOUCH_DOWN,
                    .touch.x=g_last_x,
                    .touch.y=g_last_y
            };
            mgl_dispatch_touch_event(root,&event);
            g_touch_pressed=true;
        }

    }else{
        if(g_touch_pressed){
            mgl_event_t event={
                    .type=MGL_EVENT_TOUCH_UP,
                    .touch.x=g_last_x,
                    .touch.y=g_last_y
            };
            mgl_dispatch_touch_event(root,&event);
            g_touch_pressed=false;
        }
    }
}