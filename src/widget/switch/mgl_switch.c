#include "mgl_switch.h"
#include "draw/mgl_draw_ctx_round_rect.h"
#include "draw/mgl_draw_ctx_circle.h"
// #region draw
static void draw(mgl_draw_ctx_t *ctx){
    //拿到基类
    mgl_widget_t *self=ctx->widget;
    //获得控件
    mgl_switch_t *switch_widget= container_of(self,mgl_switch_t,base);

    //胶囊半径
    mgl_coord_t r=(mgl_coord_t)(self->bounds.h/2);

    //通过当前状态决定轨道的最终画笔
    //要注意MGL_THEME_XXX的生命周期
    const mgl_painter_t *track_painter=MGL_THEME_ACCENT();
    if(!switch_widget->value){
        track_painter=&switch_widget->track;
    }

    //画轨道
    mgl_ctx_fill_round_rect(ctx,self->bounds.x,self->bounds.y,
                            self->bounds.w,self->bounds.h,
                            r,track_painter);

    //画滑块
    //留2px间隙
    mgl_coord_t thumb_r=(mgl_coord_t)(r-2);
    mgl_coord_t cx;
    if(switch_widget->value){
        //靠右
        cx=(mgl_coord_t)(self->bounds.x+self->bounds.w-thumb_r-2);
    }else{
        //靠左
        cx=(mgl_coord_t)(self->bounds.x+thumb_r+2);
    }

    mgl_coord_t cy=(mgl_coord_t)(self->bounds.y+r);
    mgl_ctx_fill_circle(ctx,cx,cy,thumb_r,true,&switch_widget->thumb);

}
// #endregion

// #region measure
static void measure(mgl_widget_t *self,
                           mgl_measure_constraint_t cw,
                           mgl_measure_constraint_t ch,
                           mgl_coord_t *out_w,mgl_coord_t *out_h){
    //获得控件
    mgl_switch_t *switch_widget=container_of(self,mgl_switch_t,base);

    //自然宽度为轨道高度*2
    mgl_coord_t natural_w=(mgl_coord_t)(switch_widget->track_h*2);
    //自然高度为轨道高度
    mgl_coord_t natural_h=switch_widget->track_h;

    MGL_MEASURE_RESOLVE(self->pref_w,natural_w,cw,out_w);
    MGL_MEASURE_RESOLVE(self->pref_h,natural_h,ch,out_h);
}
// #endregion

// #region on_event
static bool on_event(mgl_widget_t *self,const mgl_event_t *event){
    //获得控件
    mgl_switch_t *switch_widget=container_of(self,mgl_switch_t,base);

    //如果是抬起
    if(event->type==MGL_EVENT_TOUCH_UP){
        //翻转当前状态
        switch_widget->value=!switch_widget->value;

        //标记控件脏，因为位置大小不会变，所以使用mgl_widget_set_dirty
        mgl_widget_set_dirty(self);
        return true;
    }
    return false;
}
// #endregion

// #region get_action
static mgl_action_type_t get_action(mgl_widget_t *self,const mgl_event_t *event){
    (void)self;
    //如果是抬起事件转发为值变化动作
    if(event->type==MGL_EVENT_TOUCH_UP){
        return MGL_ACTION_VALUE_CHANGED;
    }
    return MGL_ACTION_NONE;
}
// #endregion

// #region vtable
mgl_widget_vtable_t vtable={
    .draw=draw,
    .measure=measure,
    .layout=NULL,
    .on_event=on_event,
    .get_action=get_action
};
// #endregion

// #region init
void *mgl_switch_init(void *memory,const void *args){
    //获得参数结构体
    const mgl_switch_args_t *switch_args=(const mgl_switch_args_t *)args;
    //获得控件
    mgl_switch_t *switch_widget= container_of(memory,mgl_switch_t,base);
    //初始化基类
    mgl_widget_init(&switch_widget->base,&vtable,"switch",NULL);
    //将参数结构体的字段赋值到控件
    MGL_WIDGET_BASE_FIELD_HANDLE(switch_widget,switch_args);

    MGL_WIDGET_FIELD_HANDLE(switch_widget,switch_args,value);
    MGL_WIDGET_FIELD_HANDLE_DEFAULT(switch_widget,switch_args,track_h,MGL_SWITCH_DEFAULT_TRACK_H);

    //轨道画笔，默认为主题轨道画笔
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME_DEFAULT(switch_widget,switch_args,track,MGL_THEME_TRACK());
    //滑块画笔，默认为主题背景画笔
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME_DEFAULT(switch_widget,switch_args,thumb,MGL_THEME_BG());
    //返回控件基类地址
    return &switch_widget->base;
}
// #endregion