#include "mgl_event.h"
#include "logger/mgl_log.h"
static struct {
    uint8_t finger_id;
    mgl_coord_t down_x,down_y;
    mgl_coord_t x,y;
} fingers[MGL_TOUCH_MAX_FINGERS];
static uint8_t finger_count=0;

static int find_finger_idx(uint8_t id){
    for (int i=0;i<finger_count;i++){
        if(fingers[i].finger_id==id){
            return i;
        }
    }
    return -1;
}

static void add_finger(uint8_t id,mgl_coord_t x,mgl_coord_t y){
    if(finger_count<MGL_TOUCH_MAX_FINGERS){
        fingers[finger_count].finger_id=id;
        fingers[finger_count].x=x;
        fingers[finger_count].y=y;
        fingers[finger_count].down_x=x;
        fingers[finger_count].down_y=y;
        finger_count++;
    }
}

static void remove_finger(int idx){
    fingers[idx]=fingers[--finger_count];
}

static mgl_widget_t *get_deepest_hit_widget(mgl_widget_t *root,mgl_coord_t x,mgl_coord_t y){
    if(!root||!root->visible){return NULL;}

    mgl_widget_t *stack[MGL_HIT_TEST_MAX_DEPTH];
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
                if(stack_top<MGL_HIT_TEST_MAX_DEPTH){
                    stack[stack_top++]=child;
                }else{
                    MGL_LOG_ERROR(MGL_LOG_TAG_EVENT,
                                  "hit-test stack overflow, child(%p) type=%d dropped",
                                  (void*)child,child->type);
                }
                child=child->next_sibling;
            }
        }
    }
    return hit;
}

mgl_action_type_t mgl_event_default_get_action(mgl_widget_t *self,const mgl_event_t *event){
    if(event->type==MGL_EVENT_TOUCH_UP){
        return MGL_ACTION_CLICK;
    }
    return MGL_ACTION_NONE;
}

static bool event_bubble(mgl_widget_t *from,mgl_event_t *event){
    mgl_widget_t *current=from;
    while(current){
        bool consumed=false;
        if(current->vtable&&current->vtable->on_event){
            consumed=current->vtable->on_event(current,event);
            MGL_LOG_DBG(MGL_LOG_TAG_EVENT,
                        "bubble widget(%p) type=%d event=%d → %s",
                        (void*)current,current->type,event->type,
                        consumed ? "CONSUMED" : "PASS");
        }
        mgl_action_type_t action;
        if(current->vtable->get_action){
            action=current->vtable->get_action(current,event);
        }else{
            action=mgl_event_default_get_action(current,event);
        }
        if(action!=MGL_ACTION_NONE&&current->action_handler){
            consumed=current->action_handler(current,action);
        }
        if(consumed){return true;}
        current=current->parent;
    }
    return false;
}

bool mgl_dispatch_touch_event(mgl_widget_t *root,mgl_event_t *event){
    mgl_widget_t *target= get_deepest_hit_widget(root,event->touch.x,event->touch.y);
    if(!target){return false;}
    return event_bubble(target, event);
}

void mgl_process_touch_data(const mgl_touch_data_t *data,mgl_widget_t *root){
    //抬起的手指，如果当前data中不存在则UP
    for(int i =0; i < finger_count;){
        bool alive=false;
        if(data){
            for(uint8_t n=0;n<data->count;n++){
                if(data->points[n].finger_id == fingers[i].finger_id){
                    alive=true;
                    break;
                }
            }
        }
        if(!alive){
            mgl_widget_t *target=get_deepest_hit_widget(root,
                                                        fingers[i].down_x,
                                                        fingers[i].down_y);
            if(target){
                mgl_event_t event={
                        .type=MGL_EVENT_TOUCH_UP,
                        .touch.x=fingers[i].x,
                        .touch.y=fingers[i].y
                };
                event_bubble(target,&event);
            }
            remove_finger(i);
        }else{
            i++;
        }
    }

    //当前手指，如果新增则DOWN，如果已存在且移动则为MOVE
    if(!data){return;}
    for(uint8_t i=0;i<data->count;i++){
        uint8_t id=data->points[i].finger_id;
        int idx=find_finger_idx(id);
        if(idx<0){
            //新增
            add_finger(id,data->points[i].x,data->points[i].y);
            mgl_event_t event={
                    .type=MGL_EVENT_TOUCH_DOWN,
                    .touch.x=data->points[i].x,
                    .touch.y=data->points[i].y
            };
            mgl_dispatch_touch_event(root,&event);
        }else if(data->points[i].x != fingers[idx].x ||
                 data->points[i].y != fingers[idx].y){
            //移动
            mgl_widget_t *target = get_deepest_hit_widget(root,
                                                          fingers[idx].down_x,
                                                          fingers[idx].down_y);
            if(target){
                mgl_event_t event={
                        .type=MGL_EVENT_TOUCH_MOVE,
                        .touch.x=data->points[i].x,
                        .touch.y=data->points[i].y
                };
                event_bubble(target,&event);
            }
            fingers[idx].x=data->points[i].x;
            fingers[idx].y=data->points[i].y;
        }
    }
}