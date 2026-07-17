#include "mgl_scrollbar.h"

static void draw(mgl_draw_ctx_t *ctx){
    mgl_widget_t *self=ctx->widget;
    mgl_scrollbar_t *sb=container_of(self,mgl_scrollbar_t,base);

    //内容不满屏不显示滚动条
    if(sb->content_total<=sb->viewport_h||sb->viewport_h<=0){
        return;
    }

    int32_t thumb_h=(int32_t)sb->viewport_h*sb->viewport_h/sb->content_total;
    if(thumb_h<MGL_SCROLLBAR_DEFAULT_THUMB_MIN_H){
        thumb_h = MGL_SCROLLBAR_DEFAULT_THUMB_MIN_H;
    }
    if(thumb_h>sb->viewport_h){
        thumb_h=sb->viewport_h;
    }

    int32_t max_scroll=sb->content_total-sb->viewport_h;
    int32_t thumb_y;
    if(max_scroll>0){
        thumb_y=(int32_t)sb->position*(sb->viewport_h-thumb_h)/max_scroll;
    }else {
        thumb_y=0;
    }
    mgl_coord_t bx=(mgl_coord_t)(self->bounds.x+self->bounds.w-sb->bar_w);
    mgl_coord_t by=(mgl_coord_t)(self->bounds.y+thumb_y);

    //track轨道背景
    mgl_ctx_fill_rect(ctx,bx,self->bounds.y,sb->bar_w,
                      (mgl_coord_t)self->bounds.h,&sb->track);

    //thumb滑块
    mgl_ctx_fill_round_rect(ctx,bx,by,sb->bar_w,
                            (mgl_coord_t)thumb_h,(mgl_coord_t)(sb->bar_w/2),
                            &sb->thumb);
}

static void measure(mgl_widget_t *self,
                              mgl_measure_constraint_t cw,
                              mgl_measure_constraint_t ch,
                              mgl_coord_t *out_w,mgl_coord_t *out_h){
    mgl_scrollbar_t *sb=container_of(self,mgl_scrollbar_t,base);
    mgl_coord_t natural_w=sb->bar_w;
    MGL_MEASURE_RESOLVE(self->pref_w,natural_w,cw,out_w);
    MGL_MEASURE_RESOLVE(self->pref_h,100,ch,out_h);
}

static int32_t scrollbar_y_to_position(mgl_scrollbar_t *sb,mgl_coord_t y){
    int32_t thumb_h=(int32_t)sb->viewport_h*sb->viewport_h/sb->content_total;
    if(thumb_h<MGL_SCROLLBAR_DEFAULT_THUMB_MIN_H){
        thumb_h=MGL_SCROLLBAR_DEFAULT_THUMB_MIN_H;
    }
    if (thumb_h>sb->viewport_h){
        thumb_h=sb->viewport_h;
    }

    int32_t max_scroll=sb->content_total-sb->viewport_h;
    if(max_scroll<=0){return 0;}

    int32_t rel_y=y-sb->base.bounds.y-thumb_h/2;
    if(rel_y<0){rel_y=0;}
    if(rel_y>sb->viewport_h-thumb_h){
        rel_y=sb->viewport_h-thumb_h;
    }

    return rel_y*max_scroll/(sb->viewport_h-thumb_h);
}

static bool on_event(mgl_widget_t *self, const mgl_event_t *event) {
    mgl_scrollbar_t *sb = container_of(self, mgl_scrollbar_t, base);
    //内容不满屏时不响应
    if(sb->content_total<=sb->viewport_h){return false;}

    int32_t thumb_h=(int32_t)sb->viewport_h*sb->viewport_h/sb->content_total;
    if(thumb_h<MGL_SCROLLBAR_DEFAULT_THUMB_MIN_H){
        thumb_h=MGL_SCROLLBAR_DEFAULT_THUMB_MIN_H;
    }

    int32_t max_scroll=sb->content_total-sb->viewport_h;
    int32_t thumb_y=0;
    if(max_scroll>0){
        thumb_y=(int32_t)sb->position*(sb->viewport_h-thumb_h)/max_scroll;
    }

    mgl_coord_t thumb_top=(mgl_coord_t)(self->bounds.y+thumb_y);
    mgl_coord_t thumb_bot=(mgl_coord_t)(thumb_top+thumb_h);
    int32_t new_pos;
    switch(event->type){
        case MGL_EVENT_TOUCH_DOWN:
            sb->dragging=true;
            sb->touch_start_y=event->touch.y;
            sb->position_at_start=sb->position;
            new_pos=scrollbar_y_to_position(sb, event->touch.y);
            if(new_pos!=sb->position&&sb->on_scroll){
                sb->position=new_pos;
                sb->on_scroll(new_pos,sb->callback_data);
            }
            mgl_widget_set_dirty(self);
            return true;
        case MGL_EVENT_TOUCH_MOVE:{
            if(!sb->dragging){return false;}

            new_pos=scrollbar_y_to_position(sb, event->touch.y);
            if(new_pos!=sb->position&&sb->on_scroll){
                sb->position=new_pos;
                sb->on_scroll(new_pos,sb->callback_data);
                mgl_widget_set_dirty(self);
            }
            return true;
        }
        case MGL_EVENT_TOUCH_UP:
            sb->dragging=false;
            return true;

        default:
            return false;
    }
}

static const mgl_widget_vtable_t vtable={
        .draw=draw,
        .on_event=on_event,
        .measure=measure
};

void *mgl_scrollbar_init(void *memory,const void *args){
    const mgl_scrollbar_args_t *sa=(const mgl_scrollbar_args_t *)args;
    mgl_scrollbar_t *sb=(mgl_scrollbar_t *)memory;

    mgl_widget_init(&sb->base,&vtable,NULL,NULL,MGL_WIDGET_TYPE_SCROLLBAR);

    MGL_WIDGET_FIELD_HANDLE_DEFAULT(sb,sa,bar_w,MGL_SCROLLBAR_DEFAULT_BAR_W);
    sb->dragging=false;
    sb->on_scroll=sa->on_scroll;
    sb->callback_data=sa->callback_data;

    MGL_WIDGET_BASE_FIELD_HANDLE(sb,sa);
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME_DEFAULT(sb,sa,track,MGL_THEME_TRACK());
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME_DEFAULT(sb,sa,thumb,MGL_THEME_BG());

    return &sb->base;
}

void mgl_scrollbar_set_content(mgl_scrollbar_t *sb,
                               int32_t content_total,int32_t viewport_h,
                               int32_t position){
    sb->content_total=content_total;
    sb->viewport_h=viewport_h;
    sb->position=position;
    mgl_widget_set_dirty(&sb->base);
}