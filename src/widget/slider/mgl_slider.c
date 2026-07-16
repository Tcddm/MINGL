#include "mgl_slider.h"

static mgl_coord_t value_to_thumb_x(mgl_slider_t *s,mgl_coord_t bw){
    int32_t range=(int32_t)s->max-s->min;
    if(range<=0){return s->base.bounds.x;}
    return (mgl_coord_t)(s->base.bounds.x+
                         (int32_t)(bw-s->thumb_w)*(s->value-s->min)/range);
}

static int16_t point_to_value(mgl_slider_t *s,mgl_coord_t x){
    mgl_coord_t bw=s->base.bounds.w;
    int32_t range_w=(int32_t)bw-s->thumb_w;
    if(range_w<=0){return s->min;}
    int32_t rel=(int32_t)x-s->base.bounds.x-s->thumb_w/2;
    if(rel<0){rel=0;}
    if(rel>range_w){rel=range_w;}
    int32_t value_range=(int32_t)s->max-s->min;
    return (int16_t)(s->min+rel*value_range/range_w);
}

static void draw(mgl_draw_ctx_t *ctx){
    mgl_widget_t *self=ctx->widget;
    mgl_slider_t *s=container_of(self, mgl_slider_t, base);

    mgl_coord_t bw=self->bounds.w;
    mgl_coord_t bx=self->bounds.x;
    mgl_coord_t track_y=(mgl_coord_t)(self->bounds.y+
                                        (self->bounds.h-s->track_h)/2);

    //track轨道
    mgl_ctx_fill_rect(ctx,bx,track_y,bw,s->track_h,
                            &s->track);

    //fill已填充部分
    mgl_coord_t thumb_x=value_to_thumb_x(s,bw);
    mgl_coord_t fill_w=(mgl_coord_t)(thumb_x-bx+s->thumb_w/2);
    if(fill_w>0){
        mgl_ctx_fill_rect(ctx,bx,track_y,fill_w,s->track_h,&s->fill);
    }

    //thumb 滑块
    mgl_coord_t thumb_y=(mgl_coord_t)(self->bounds.y+
                                        (self->bounds.h-s->thumb_h)/2);
    mgl_ctx_fill_round_rect(ctx,thumb_x,thumb_y,
                            s->thumb_w,s->thumb_h,s->round_radius,&s->thumb);
}

static void measure(mgl_widget_t *self,
                           mgl_measure_constraint_t cw,
                           mgl_measure_constraint_t ch,
                           mgl_coord_t *out_w,mgl_coord_t *out_h){
    mgl_slider_t *s=container_of(self, mgl_slider_t, base);
    mgl_coord_t natural_h;
    if(s->track_h>s->thumb_h){
        natural_h=s->track_h;
    }else{
        natural_h=s->thumb_h;
    }
    MGL_MEASURE_RESOLVE(self->pref_w,160,cw,out_w);
    MGL_MEASURE_RESOLVE(self->pref_h,natural_h,ch,out_h);
}

static bool on_event(mgl_widget_t *self,const mgl_event_t *event){
    mgl_slider_t *s=container_of(self, mgl_slider_t, base);

    switch(event->type){
        case MGL_EVENT_TOUCH_DOWN:
            s->dragging=true;
            goto update_value;

        case MGL_EVENT_TOUCH_MOVE:
            if(!s->dragging) return false;
            goto update_value;

        case MGL_EVENT_TOUCH_UP:
            s->dragging=false;
            mgl_widget_set_dirty(self);
            return true;

        default:
            return false;
    }

    update_value:{
    int16_t new_val=point_to_value(s,event->touch.x);
    if(new_val!=s->value){
        s->value=new_val;
        mgl_widget_set_dirty(self);
    }
    return true;
}
}

static mgl_action_type_t get_action(mgl_widget_t *self,const mgl_event_t *event){
    if(event->type==MGL_EVENT_TOUCH_MOVE||
        event->type==MGL_EVENT_TOUCH_UP)
        return MGL_ACTION_VALUE_CHANGED;
    return MGL_ACTION_NONE;
}

static const mgl_widget_vtable_t vtable={
        .draw=draw,
        .on_event=on_event,
        .measure=measure,
        .get_action=get_action
};

void *mgl_slider_init(void *memory,const void *args){
    const mgl_slider_args_t *sa=(const mgl_slider_args_t *)args;
    mgl_slider_t *s=(mgl_slider_t *)memory;

    mgl_widget_init(&s->base,&vtable,NULL,NULL,MGL_WIDGET_TYPE_SLIDER);

    s->value=sa->value;
    s->min=sa->min;

    MGL_WIDGET_FIELD_DEFAULT_VALUE(s,sa,max,MGL_SLIDER_DEFAULT_MAX);
    MGL_WIDGET_FIELD_DEFAULT_VALUE(s,sa,track_h,MGL_SLIDER_DEFAULT_TRACK_H);
    MGL_WIDGET_FIELD_DEFAULT_VALUE(s,sa,thumb_w,MGL_SLIDER_DEFAULT_THUMB_H);
    MGL_WIDGET_FIELD_DEFAULT_VALUE(s,sa,thumb_h,MGL_SLIDER_DEFAULT_THUMB_H);

    s->dragging=false;

    MGL_WIDGET_BASE_FIELD_HANDLE(s,sa);
    MGL_WIDGET_ROUND_RADIUS_FIELD_HANDLE(s,sa);
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(s,sa,track);
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(s,sa,fill);
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(s,sa,thumb);

    return &s->base;
}