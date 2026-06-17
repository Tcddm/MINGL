#include "mgl_list.h"
#include "pool/mgl_page_pool.h"

static int32_t compute_cumulative_y(const mgl_list_t *list,uint16_t index){
    int32_t y=0;
    for(uint16_t i=0;i<index&&i<list->item_count;i++){
        y+=list->item_heights[i];
    }
    return y;
}

static void sync_scrollbar(mgl_list_t *list){
    if(!list->scrollbar.on_scroll){return;}
    mgl_scrollbar_set_content(&list->scrollbar,
                              list->content_height,
                              list->base.bounds.h,
                              list->scroll_y);
    list->scrollbar.base.visible=
            (list->content_height>list->base.bounds.h);
}

static void list_recycle(mgl_list_t *list){
    uint16_t total=list->item_count;
    uint8_t pool_n=list->pool_size;
    mgl_coord_t view_h=list->base.bounds.h;
    if(view_h<=0){
        if(list->base.pref_h>0){
            view_h=list->base.pref_h;
        }else{
            view_h=320;
        }
    }

    if(total==0||pool_n==0){return;}

    //标记所有槽位未使用
    for(uint8_t s=0;s<pool_n;s++){
        //高位标记
        list->pool[s].list_index |= 0x8000;
    }

    //计算可见item范围
    int32_t vis_top=list->scroll_y;
    int32_t vis_bot=vis_top+view_h;
    int32_t cum_y=0;
    uint16_t start_idx=total,end_idx=0;

    for(uint16_t i=0;i<total;i++){
        int16_t h=list->item_heights[i];
        //未初始化占位
        if(h==0){h=50;}

        if(start_idx==total&&cum_y+h>vis_top){
            start_idx=i;
        }
        if(cum_y<vis_bot){
            end_idx=i;
        }else{
            break;
        }
        cum_y+=h;
    }
    if(start_idx == total){
        start_idx=total-1;
        end_idx=total-1;
    }
    if(end_idx>=total){
        end_idx=total-1;
    }

    //遍历可见index，分配槽位
    for(uint16_t i=start_idx;i<=end_idx;i++){
        //查找已绑定此index的槽位
        uint8_t found=pool_n;
        for(uint8_t s=0;s<pool_n;s++){
            if(list->pool[s].list_index==i){
                found=s;
                break;
            }
        }

        //未绑定则找空闲槽位
        if(found==pool_n){
            for(uint8_t s=0;s<pool_n;s++){
                if(list->pool[s].list_index==MGL_LIST_INVALID_INDEX){
                    found=s;
                    break;
                }
            }
        }

        //仍然没找到则取最早未使用的（替换）
        if(found==pool_n){
            for(uint8_t s=0;s<pool_n;s++){
                if(list->pool[s].list_index & 0x8000){
                    found=s;
                    break;
                }
            }
        }

        if(found==pool_n){continue;}


        list->pool[found].list_index=i;
        list->adapter.bind(list->adapter.user_data,
                           list->pool[found].root,i);

        //高度若未初始化则自动测量
        if(list->item_heights[i]==0){
            mgl_coord_t h=list->adapter.get_height(
                    list->adapter.user_data,i);
            if(h>0){
                list->item_heights[i]=h;
                list->content_height=compute_cumulative_y(list,total);
            }else{
                mgl_coord_t nw,nh;
                list->pool[found].root->vtable->measure(
                        list->pool[found].root,
                        (mgl_measure_constraint_t){list->base.bounds.w,
                                                   MGL_MEASURE_AT_MOST},
                        (mgl_measure_constraint_t){32767,MGL_MEASURE_NONE},
                        &nw,&nh);
                list->item_heights[i]=nh;
                list->content_height=compute_cumulative_y(list,total);
            }
        }

        if(!list->pool[found].root){continue;}

        //定位
        int32_t slot_y=compute_cumulative_y(list,i);
        list->pool[found].root->bounds.x=list->base.bounds.x;
        list->pool[found].root->bounds.y=
                (mgl_coord_t)(list->base.bounds.y+slot_y-list->scroll_y);
        list->pool[found].root->bounds.w=
                (mgl_coord_t)(list->base.bounds.w-list->scrollbar.bar_w);
        list->pool[found].root->bounds.h=list->item_heights[i];

        if(list->pool[found].root->vtable&&
            list->pool[found].root->vtable->layout){
            list->pool[found].root->vtable->layout(
                    list->pool[found].root,
                    &list->pool[found].root->bounds);
        }
        list->pool[found].root->visible = true;
    }

    //释放仍高位标记的槽位
    for(uint8_t s=0;s<pool_n;s++){
        if(list->pool[s].list_index & 0x8000){
            list->pool[s].root->visible=false;
            list->pool[s].list_index=MGL_LIST_INVALID_INDEX;
        }
    }

    //scrollbar定位
    if(list->scrollbar.on_scroll){
        list->scrollbar.base.bounds.x=
                (mgl_coord_t)(list->base.bounds.x+list->base.bounds.w
                              -list->scrollbar.bar_w);
        list->scrollbar.base.bounds.y=list->base.bounds.y;
        list->scrollbar.base.bounds.w=list->scrollbar.bar_w;
        list->scrollbar.base.bounds.h=list->base.bounds.h;
    }
}

static void on_scrollbar_move(int32_t position,void *user_data){
    mgl_list_t *list=(mgl_list_t *)user_data;
    int32_t max_y=list->content_height-list->base.bounds.h;
    if(max_y<0){max_y=0;}
    if(position<0){position=0;}
    if(position>max_y){position=max_y;}
    list->scroll_y=position;
    list_recycle(list);
    mgl_widget_set_dirty(&list->base);
}

static void draw(mgl_draw_ctx_t *ctx){
    mgl_widget_t *self=ctx->widget;
    mgl_list_t *list=container_of(self,mgl_list_t,base);
    if(list->painter.type!=MGL_PAINTER_TYPE_EMPTY){
        mgl_ctx_fill_rect(ctx,self->bounds.x,self->bounds.y,
                          self->bounds.w,self->bounds.h,&list->painter);
    }

}

static void measure(mgl_widget_t *self,
                         mgl_measure_constraint_t cw,
                         mgl_measure_constraint_t ch,
                         mgl_coord_t *out_w,mgl_coord_t *out_h){
    mgl_list_t *list=container_of(self,mgl_list_t,base);
    MGL_MEASURE_RESOLVE(self->pref_w,self->bounds.w,cw,out_w);
    MGL_MEASURE_RESOLVE(self->pref_h,
                        (mgl_coord_t)list->content_height,ch,out_h);
}

static bool on_event(mgl_widget_t *self,const mgl_event_t *event){
    mgl_list_t *list=container_of(self,mgl_list_t,base);
    int32_t max_y=list->content_height-self->bounds.h;
    if(max_y<0){max_y=0;}
    switch(event->type){
        case MGL_EVENT_TOUCH_DOWN:
            list->touch_start_y=event->touch.y;
            list->scroll_at_start=list->scroll_y;
            list->velocity=0;
            list->animating=0;
            return true;

        case MGL_EVENT_TOUCH_MOVE:{
            int32_t delta=(int32_t)(list->touch_start_y-event->touch.y);
            int32_t new_y=list->scroll_at_start+delta;
            if(new_y<0){new_y=0;}
            if(new_y>max_y){new_y=max_y;}

            if(new_y!=list->scroll_y){
                list->scroll_y=new_y;
                list_recycle(list);
                sync_scrollbar(list);
                mgl_widget_set_dirty(self);
            }
            return true;
        }
        case MGL_EVENT_TOUCH_UP:
            list->animating=0;
            return true;
        default:
            return false;
    }
}

static void layout(mgl_widget_t *self,const mgl_rect_t *area){
    mgl_list_t *list=container_of(self,mgl_list_t,base);
    self->bounds=*area;

    if(list->scrollbar.on_scroll){
        list->scrollbar.base.bounds=(mgl_rect_t){
                (mgl_coord_t)(area->x+area->w-list->scrollbar.bar_w),
                area->y,
                list->scrollbar.bar_w,area->h
        };
        list->scrollbar.base.visible=
                (list->content_height>area->h);
    }
    for(uint8_t i=0;i<list->pool_size;i++){
        if(list->pool[i].list_index==MGL_LIST_INVALID_INDEX){
            continue;
        }
        if(!list->pool[i].root||!list->pool[i].root->visible){
            continue;
        }
        int32_t slot_y=compute_cumulative_y(list,list->pool[i].list_index);
        list->pool[i].root->bounds.x=area->x;
        list->pool[i].root->bounds.y=
                (mgl_coord_t)(area->y+slot_y-list->scroll_y);
        list->pool[i].root->bounds.w=
                (mgl_coord_t)(area->w-list->scrollbar.bar_w);
    }
    sync_scrollbar(list);
}

static const mgl_widget_vtable_t vtable={
        .draw=draw,
        .on_event=on_event,
        .measure=measure,
        .layout=layout
};

void *mgl_list_init(void *memory,const void *args){
    const mgl_list_args_t *la=(const mgl_list_args_t *)args;
    mgl_list_t *list=(mgl_list_t *)memory;

    mgl_widget_init(&list->base,&vtable,
                    NULL,NULL,MGL_WIDGET_TYPE_LIST);

    list->adapter=*la->adapter;
    list->item_blueprint=la->item_blueprint;

    MGL_WIDGET_BASE_FIELD_HANDLE(list,la);
    MGL_WIDGET_PAINTER_FIELD_HANDLE(list,la);
    MGL_WIDGET_ROUND_RADIUS_FIELD_HANDLE(list,la);
    MGL_WIDGET_ACTION_HANDLER_FIELD_HANDLE(list,la);

    //item_count
    list->item_count=list->adapter.get_count(list->adapter.user_data);
    if(list->item_count==0){goto init_scrollbar;}

    //分配高度数组
    list->item_heights=(int16_t *)mgl_page_pool_alloc(
            sizeof(int16_t)*list->item_count);
    if(!list->item_heights){goto init_scrollbar;}
    for(uint16_t i=0;i<list->item_count;i++){
        list->item_heights[i]=0;
    }

    //预加载固定高度项
    for(uint16_t i=0;i<list->item_count;i++){
        mgl_coord_t h=list->adapter.get_height(list->adapter.user_data,i);
        if(h<=0){break;}
        list->item_heights[i]=h;
        list->content_height+=h;
    }

    mgl_coord_t min_h=50;
    for(uint16_t i=0;i<list->item_count&&i<5;i++){
        if(list->item_heights[i]>0&&list->item_heights[i]<min_h){
            min_h=list->item_heights[i];
        }
    }
    mgl_coord_t view_h;
    if(list->base.pref_h>0){
        view_h=list->base.pref_h;
    }else{
        view_h=320;
    }
    uint8_t visible=(uint8_t)((view_h+min_h-1)/min_h);
    if(visible<1){visible=1;}
    list->pool_size=visible+1;
    if(list->pool_size<MGL_LIST_POOL_MIN_SIZE){
        list->pool_size=MGL_LIST_POOL_MIN_SIZE;
    }

    //分配池
    list->pool=(mgl_list_slot_t *)mgl_page_pool_alloc(
            sizeof(mgl_list_slot_t)*list->pool_size);
    if(!list->pool){
        list->pool_size=0;
        goto init_scrollbar;
    }

    for(uint8_t s=0;s<list->pool_size;s++){
        list->pool[s].root=mgl_blueprint_collect(
                list->item_blueprint,&list->base);
        if(!list->pool[s].root){
            list->pool[s].list_index = MGL_LIST_INVALID_INDEX;
            continue;
        }
        list->pool[s].root->visible=false;
        list->pool[s].list_index=MGL_LIST_INVALID_INDEX;
    }

    list_recycle(list);

    init_scrollbar:
    //内嵌scrollbar
    mgl_scrollbar_init(&list->scrollbar,&la->scrollbar_args);
    mgl_list_set_scrollbar(list,&list->scrollbar);
    list->scrollbar.base.bounds=(mgl_rect_t){
            (mgl_coord_t)(list->base.bounds.x+list->base.bounds.w
                          -list->scrollbar.bar_w),
            list->base.bounds.y,
            list->scrollbar.bar_w,
            list->base.bounds.h
    };
    mgl_widget_add_child(&list->base,&list->scrollbar.base);
    return &list->base;
}

void mgl_list_set_scrollbar(mgl_list_t *list,mgl_scrollbar_t *sb){
    list->scrollbar=*sb;
    sb->on_scroll=on_scrollbar_move;
    sb->callback_data=list;
    sync_scrollbar(list);
}

void mgl_list_notify_height_changed(mgl_list_t *list,uint16_t index,
                                    int16_t new_h){
    if(index>=list->item_count){return;}
    if (new_h<=list->item_heights[index]){return;}

    list->content_height-=list->item_heights[index];
    list->item_heights[index]=new_h;
    list->content_height+=new_h;
    list_recycle(list);
    sync_scrollbar(list);
    mgl_widget_set_dirty(&list->base);
}

void mgl_list_scroll_to(mgl_list_t *list,uint16_t index){
    if(index>=list->item_count){return;}
    int32_t target_y=compute_cumulative_y(list,index);
    int32_t max_y=list->content_height-list->base.bounds.h;
    if(max_y<0){max_y=0;}
    if(target_y>max_y){target_y=max_y;}
    list->scroll_y=target_y;
    list_recycle(list);
    sync_scrollbar(list);
    mgl_widget_set_dirty(&list->base);
}

int32_t mgl_list_get_scroll_position(const mgl_list_t *list){
    return list->scroll_y;
}

mgl_widget_t *mgl_list_get_slot_root(const mgl_list_t *list,
                                     uint16_t index){
    for(uint8_t s=0;s<list->pool_size;s++)
        if(list->pool[s].list_index==index) {
            return list->pool[s].root;
        }
    return NULL;
}