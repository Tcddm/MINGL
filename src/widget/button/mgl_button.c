#include "mgl_button.h"
#include "widget/mgl_layout_utils.h"


static void button_draw(mgl_draw_ctx_t *ctx){
    mgl_widget_t *self=ctx->widget;
    mgl_button_t *button=container_of(self,mgl_button_t,base);
    mgl_painter_t bg=button->background;
    if(button->state==MGL_BUTTON_STATE_PRESSED){
        bg.solid_color.value=(bg.solid_color.value>>1)&0x7BEF;
    }
    mgl_ctx_fill_round_rect(ctx,self->bounds.x,self->bounds.y,
                                self->bounds.w,self->bounds.h,
                                button->round_radius,&bg);
    mgl_coord_t content_x=(mgl_coord_t)(self->bounds.x+button->padding);
    mgl_coord_t content_y=(mgl_coord_t)(self->bounds.y+button->padding);
    mgl_coord_t content_w=(mgl_coord_t)(self->bounds.w-button->padding*2);
    mgl_coord_t content_h=(mgl_coord_t)(self->bounds.h-button->padding*2);

    mgl_layout_item_t items[2];
    uint8_t count=0;

    if(button->bmp){
        items[count].width=button->bmp->w;
        items[count].height=button->bmp->h;
        count++;
    }

    if(button->text&&button->font){
        items[count].width=mgl_font_get_text_width(button->font,button->text);
        items[count].height=button->font->font_size;
        count++;
    }

    mgl_coord_t cross=0;
    for(uint8_t i=0;i<count;i++){
        if(items[i].height>cross){cross=items[i].height;}
    }

    mgl_layout_linear_layout(items,count,MGL_LINEAR_HORIZONTAL,button->spacing,
                             MGL_ALIGN_CENTER,
                             MGL_ALIGN_CENTER,
                             content_x,content_y,content_h,content_w);

    uint8_t idx=0;

    if(button->bmp){
        mgl_coord_t icon_y=(mgl_coord_t)(content_y+(content_h-button->bmp->h)/2);
        mgl_ctx_draw_bitmap(ctx,items[idx].bounds.x,icon_y,
                            button->bmp,MGL_COLOR_TRANSPARENT);
        idx++;
    }

    if (button->text&&button->font) {
        mgl_coord_t baseline_y=(mgl_coord_t)(items[idx].bounds.y+button->font->baseline);
        mgl_ctx_draw_text(ctx,items[idx].bounds.x,baseline_y,0,
                          button->text,button->font,&button->foreground);
    }


}

static void button_measure(mgl_widget_t *self,
                           mgl_measure_constraint_t cw,
                           mgl_measure_constraint_t ch,
                           mgl_coord_t *out_w,mgl_coord_t *out_h){
    mgl_button_t *button=container_of(self,mgl_button_t,base);

    mgl_layout_item_t items[2];
    uint8_t count=0;

    if(button->bmp){
        items[count].width=button->bmp->w;
        items[count].height=button->bmp->h;
        count++;
    }

    if(button->text&&button->font){
        items[count].width =mgl_font_get_text_width(button->font,button->text);
        items[count].height=button->font->font_size;
        count++;
    }

    mgl_coord_t content_w=0,content_h=0;
    if(count>0){
        mgl_layout_linear_measure(items,count,MGL_LINEAR_HORIZONTAL,
                                  button->spacing,&content_w,&content_h);
    }


    mgl_coord_t natural_w=(mgl_coord_t)(content_w+button->padding*2);
    mgl_coord_t natural_h=(mgl_coord_t)(content_h+button->padding*2);

    MGL_MEASURE_RESOLVE(self->pref_w, natural_w, cw, out_w);
    MGL_MEASURE_RESOLVE(self->pref_h, natural_h, ch, out_h);
}

static bool button_on_event(mgl_widget_t *self,const mgl_event_t *event){
    mgl_button_t *button=container_of(self,mgl_button_t,base);

    if(event->type==MGL_EVENT_TOUCH_DOWN){
        button->state=MGL_BUTTON_STATE_PRESSED;
        mgl_widget_set_dirty(self);
        return true;
    }

    if(event->type==MGL_EVENT_TOUCH_UP){
        button->state=MGL_BUTTON_STATE_NORMAL;
        mgl_widget_set_dirty(self);
    }

    bool user_handled=false;
    MGL_WIDGET_ACTION_HANDLE_ACTION(button,self,event,user_handled);

    return user_handled;
}

static const mgl_widget_vtable_t vtable={
        .draw=button_draw,
        .on_event=button_on_event,
        .measure=button_measure,
        .layout=NULL
};
void *mgl_button_init(void *memory,const void *args){
    const mgl_button_args_t *button_args=(const mgl_button_args_t *)args;
    mgl_button_t *button=(mgl_button_t *)memory;

    mgl_widget_init(&button->base,&vtable,NULL,NULL,MGL_WIDGET_TYPE_BUTTON);


    button->text=button_args->text;
    button->font=button_args->font;
    button->bmp=button_args->bmp;
    button->spacing=button_args->spacing;
    button->padding=button_args->padding;

    MGL_WIDGET_BASE_FIELD_HANDLE(button,button_args);
    MGL_WIDGET_ROUND_RADIUS_FIELD_HANDLE(button,button_args);
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(button,button_args,foreground);
    MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(button,button_args,background);
    MGL_WIDGET_ACTION_HANDLER_FIELD_HANDLE(button,button_args);

    return &button->base;
}