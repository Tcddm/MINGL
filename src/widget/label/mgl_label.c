#include "mgl_label.h"

static void draw(mgl_draw_ctx_t *ctx) {
    mgl_widget_t *self=ctx->widget;
    mgl_label_t *label=container_of(self,mgl_label_t,base);
    const mgl_font_t *font=label->font;
    mgl_ctx_draw_text(ctx,self->bounds.x,(mgl_coord_t)(self->bounds.y+font->baseline),0,label->text,font,&label->painter);
}

static void measure(mgl_widget_t *self,
                          mgl_measure_constraint_t cw,
                          mgl_measure_constraint_t ch,
                          mgl_coord_t *out_w,mgl_coord_t *out_h) {
    mgl_label_t *label=container_of(self,mgl_label_t,base);
    const mgl_font_t *font=label->font;
    const char *text=label->text;
    mgl_coord_t text_w=0;
    if(font&&text){
        text_w=mgl_font_get_text_width(font,text);
    }

    mgl_coord_t text_h=font->font_size;

    MGL_MEASURE_RESOLVE(self->pref_w,text_w,cw,out_w);
    MGL_MEASURE_RESOLVE(self->pref_h,text_h,ch,out_h);
}

static const mgl_widget_vtable_t vtable={
        .draw=draw,
        .measure=measure
};
void *mgl_label_init(void *memory,const void *args){
    const mgl_label_args_t *label_args=(const mgl_label_args_t *)args;
    mgl_label_t *label=(mgl_label_t *)memory;

    mgl_widget_init(&label->base,&vtable,NULL,NULL,MGL_WIDGET_TYPE_LABEL);


    label->text=label_args->text;
    label->font=label_args->font;

    MGL_WIDGET_BASE_FIELD_HANDLE(label,label_args);
    MGL_WIDGET_PAINTER_FIELD_HANDLE(label,label_args);

    return &label->base;
}

void mgl_label_set_text(mgl_label_t *label,const char *text){
    label->text=text;
    mgl_widget_set_dirty_content(&label->base);
}