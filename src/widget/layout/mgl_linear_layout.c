#include "mgl_linear_layout.h"
#include "logger/mgl_log.h"

#define LOG_TAG MGL_LOG_TAG_WIDGET(linear_layout)

static void draw(mgl_draw_ctx_t *ctx){
    mgl_linear_layout_t *layout=container_of(ctx->widget,mgl_linear_layout_t,base);
    mgl_painter_t painter=layout->painter;
    mgl_rect_t rect=layout->base.bounds;
        mgl_ctx_fill_round_rect(ctx,
                          rect.x,
                          rect.y,
                          rect.w,
                          rect.h,
                          layout->round_radius,
                          &painter
        );

}

static void linear_layout_measure(mgl_widget_t *self,
                               mgl_measure_constraint_t cw,
                               mgl_measure_constraint_t ch,
                               mgl_coord_t *out_w,mgl_coord_t *out_h) {
    mgl_linear_layout_t *layout=container_of(self,mgl_linear_layout_t,base);


    mgl_coord_t pad_left=layout->padding_margin.left;
    mgl_coord_t pad_right=layout->padding_margin.right;
    mgl_coord_t pad_top=layout->padding_margin.top;
    mgl_coord_t pad_bottom=layout->padding_margin.bottom;

    mgl_coord_t max_content_w,max_content_h;

    if(cw.mode==MGL_MEASURE_NONE){
        max_content_w=32767;
    }else{
        max_content_w=(mgl_coord_t)(cw.value-pad_left-pad_right);
    }

    if(ch.mode==MGL_MEASURE_NONE){
        max_content_h=32767;
    }else{
        max_content_h=(mgl_coord_t)(ch.value-pad_top-pad_bottom);
    }

    MGL_LOG_DBG(LOG_TAG,"measure start (%p): cw=(%d,%d), ch=(%d,%d), max_content=%dx%d",
               layout,cw.mode,cw.value,ch.mode,ch.value,max_content_w,max_content_h);

    mgl_widget_t *child=self->first_child;
    mgl_coord_t total_w=0,total_h=0;
    mgl_widget_t *fill_child=NULL;

    while(child){
        if(!child->vtable->measure){
            child=child->next_sibling;
            continue;
        }

        bool is_fill=(layout->direction==MGL_LINEAR_HORIZONTAL)
                       ? (child->pref_w==-2) : (child->pref_h==-2);

        if(is_fill){
            fill_child=child;
            child=child->next_sibling;
            continue;
        }

        mgl_measure_constraint_t child_cw,child_ch;
        mgl_coord_t child_w,child_h;

        if(layout->direction==MGL_LINEAR_HORIZONTAL){

            child_cw.value=(mgl_coord_t)(max_content_w-total_w);
            child_cw.mode=(cw.mode==MGL_MEASURE_NONE)?MGL_MEASURE_NONE
                                                     :MGL_MEASURE_AT_MOST;

            child_ch.value=max_content_h;
            child_ch.mode=(ch.mode==MGL_MEASURE_NONE)?MGL_MEASURE_NONE
                                                     :MGL_MEASURE_AT_MOST;
        }else{

            child_ch.value=(mgl_coord_t)(max_content_h-total_h);
            child_ch.mode=(ch.mode==MGL_MEASURE_NONE)?MGL_MEASURE_NONE
                                                     :MGL_MEASURE_AT_MOST;

            child_cw.value=max_content_w;
            child_cw.mode=(cw.mode==MGL_MEASURE_NONE)?MGL_MEASURE_NONE
                                                     :MGL_MEASURE_AT_MOST;
        }

        child->vtable->measure(child,child_cw,child_ch,&child_w,&child_h);


        mgl_margin_t *m=&child->margin;
        if(layout->direction==MGL_LINEAR_HORIZONTAL){
            total_w=(mgl_coord_t)(total_w+m->left+child_w+m->right);
            mgl_coord_t child_h_total=(mgl_coord_t)(m->top+child_h+m->bottom);
            if(child_h_total>total_h){total_h=child_h_total;}
        }else{
            total_h=(mgl_coord_t)(total_h+m->top+child_h+m->bottom);
            mgl_coord_t child_w_total=(mgl_coord_t)(m->left+child_w+m->right);
            if (child_w_total>total_w){total_w=child_w_total;}
        }

        MGL_LOG_DBG(LOG_TAG,"child (%p) measured: %dx%d, margin=(%d,%d,%d,%d), total_w=%d, total_h=%d",
                    (void*)child,child_w,child_h,m->left,m->top,m->right,m->bottom,total_w,total_h);

        child=child->next_sibling;
    }

    if(fill_child){
        mgl_measure_constraint_t f_cw,f_ch;
        if(layout->direction==MGL_LINEAR_HORIZONTAL){
            f_cw.value=(mgl_coord_t)(max_content_w-total_w);
            f_cw.mode=(cw.mode==MGL_MEASURE_NONE) ? MGL_MEASURE_NONE : MGL_MEASURE_EXACT;
            f_ch.value=max_content_h;
            f_ch.mode=(fill_child->pref_h==-2) ? MGL_MEASURE_EXACT : MGL_MEASURE_AT_MOST;
        }else{
            f_ch.value=(mgl_coord_t)(max_content_h-total_h);
            f_ch.mode=(ch.mode==MGL_MEASURE_NONE) ? MGL_MEASURE_NONE : MGL_MEASURE_EXACT;
            f_cw.value=max_content_w;
            f_cw.mode=(fill_child->pref_w==-2) ? MGL_MEASURE_EXACT : MGL_MEASURE_AT_MOST;
        }

        mgl_coord_t f_w,f_h;
        fill_child->vtable->measure(fill_child,f_cw,f_ch,&f_w,&f_h);

        mgl_margin_t *m=&fill_child->margin;
        if(layout->direction==MGL_LINEAR_HORIZONTAL){
            total_w=(mgl_coord_t)(total_w+m->left+f_w+m->right);
            mgl_coord_t h_total=(mgl_coord_t)(m->top+f_h+m->bottom);
            if(h_total>total_h){total_h=h_total;}
        }else{
            total_h=(mgl_coord_t)(total_h+m->top+f_h+m->bottom);
            mgl_coord_t w_total=(mgl_coord_t)(m->left+f_w+m->right);
            if(w_total>total_w){total_w=w_total;}
        }
    }

    mgl_coord_t result_w=(mgl_coord_t)(pad_left+total_w+pad_right);
    mgl_coord_t result_h=(mgl_coord_t)(pad_top+total_h+pad_bottom);

    if((cw.mode==MGL_MEASURE_EXACT)||
    (cw.mode==MGL_MEASURE_AT_MOST&&result_w>cw.value)){
        result_w=cw.value;
    }

    if((ch.mode==MGL_MEASURE_EXACT)||
    (ch.mode==MGL_MEASURE_AT_MOST&&result_h>ch.value)){
        result_h=ch.value;
    }

    mgl_coord_t natural_w=(mgl_coord_t)(pad_left+total_w+pad_right);
    mgl_coord_t natural_h=(mgl_coord_t)(pad_top+total_h+pad_bottom);

    MGL_MEASURE_RESOLVE(self->pref_w,natural_w, cw, out_w);
    MGL_MEASURE_RESOLVE(self->pref_h,natural_h, ch, out_h);
    MGL_LOG_DBG(LOG_TAG,"measure result (%p): %dx%d",layout,result_w,result_h);
}

static void linear_layout_layout(mgl_widget_t *self, const mgl_rect_t *area) {
    mgl_linear_layout_t *layout=container_of(self,mgl_linear_layout_t,base);

    self->bounds=*area;

    mgl_coord_t content_x=(mgl_coord_t)(area->x+layout->padding_margin.left);
    mgl_coord_t content_y=(mgl_coord_t)(area->y+layout->padding_margin.top);
    mgl_coord_t content_w=(mgl_coord_t)(area->w-layout->padding_margin.left-layout->padding_margin.right);
    mgl_coord_t content_h=(mgl_coord_t)(area->h-layout->padding_margin.top-layout->padding_margin.bottom);

    mgl_coord_t total_main=0;
    mgl_widget_t *tmp=self->first_child;
    while(tmp){
        if(tmp->vtable->measure){
            bool is_fill=(layout->direction==MGL_LINEAR_HORIZONTAL)
                           ? (tmp->pref_w==-2) : (tmp->pref_h==-2);
            if(is_fill){
                tmp=tmp->next_sibling;
                continue;
            }
            mgl_coord_t cw,ch;
            tmp->vtable->measure(tmp,
                                 (mgl_measure_constraint_t){32767, MGL_MEASURE_NONE},
                                 (mgl_measure_constraint_t){32767, MGL_MEASURE_NONE},
                                 &cw,&ch);
            mgl_margin_t *m=&tmp->margin;
            if(layout->direction==MGL_LINEAR_HORIZONTAL) {
                total_main=(mgl_coord_t)(total_main+m->left+cw+m->right);
            } else {
                total_main=(mgl_coord_t)(total_main+m->top+ch+m->bottom);
            }
        }
        tmp=tmp->next_sibling;
    }

    mgl_coord_t main_size;
    if(layout->direction==MGL_LINEAR_HORIZONTAL){
        main_size=content_w;
    }else{
        main_size=content_h;
    }
    mgl_coord_t start_offset=mgl_layout_align_offset(layout->main_align,
                                                     main_size,
                                                     total_main);

    MGL_LOG_DBG(LOG_TAG,"layout start (%p): area=(%d,%d,%d,%d) content=(%d,%d,%d,%d) start_offset=%d",
                layout,area->x,area->y,area->w,area->h,content_x,content_y,content_w,content_h,start_offset);

    mgl_widget_t *child=self->first_child;
    mgl_coord_t cursor=start_offset;
    mgl_widget_t *fill_child=NULL;

    while(child){

        if(!child->vtable->measure){
            child=child->next_sibling;
            continue;
        }

        bool is_fill=(layout->direction==MGL_LINEAR_HORIZONTAL)
                       ? (child->pref_w==-2) : (child->pref_h==-2);

        if(is_fill){
            fill_child=child;
            child=child->next_sibling;
            continue;
        }

        mgl_measure_constraint_t child_cw,child_ch;
        if(layout->direction==MGL_LINEAR_HORIZONTAL){
            child_cw.value=(mgl_coord_t)(content_w-cursor);
            child_cw.mode=MGL_MEASURE_AT_MOST;
            child_ch.value=content_h;
            child_ch.mode=MGL_MEASURE_AT_MOST;
        }else{
            child_cw.value=content_w;
            child_cw.mode=MGL_MEASURE_AT_MOST;
            child_ch.value=(mgl_coord_t)(content_h-cursor);
            child_ch.mode=MGL_MEASURE_AT_MOST;
        }

        mgl_coord_t child_w=0,child_h=0;

        child->vtable->measure(child,child_cw,child_ch,&child_w,&child_h);

        mgl_margin_t *m=&child->margin;
        mgl_rect_t child_area;

        if(layout->direction==MGL_LINEAR_HORIZONTAL){
            mgl_coord_t offset=mgl_layout_align_offset_with_margin(
                    layout->cross_align,content_h,child_h,m->top,m->bottom);
            child_area.x=(mgl_coord_t)(content_x+cursor+m->left);
            child_area.y=(mgl_coord_t)(content_y+offset);
            child_area.w=child_w;
            child_area.h=child_h;
            cursor=(mgl_coord_t)(cursor+m->left+child_w+m->right);
        }else{
            mgl_coord_t offset=mgl_layout_align_offset_with_margin(
                    layout->cross_align, content_w,child_w,m->left,m->right);
            child_area.x=(mgl_coord_t)(content_x+offset);
            child_area.y=(mgl_coord_t)(content_y+cursor+m->top);
            child_area.w=child_w;
            child_area.h=child_h;
            cursor=(mgl_coord_t)(cursor+m->top+child_h+m->bottom);
        }

        mgl_rect_t old_bounds=child->bounds;

        if(child->vtable&&child->vtable->layout){
            child->prev_bounds=child->bounds;
            child->vtable->layout(child,&child_area);
        }else{
            child->prev_bounds=child->bounds;
            child->bounds=child_area;
        }

        bool child_moved=(child->bounds.x!=old_bounds.x||
                            child->bounds.y!=old_bounds.y||
                            child->bounds.w!= old_bounds.w||
                            child->bounds.h!=old_bounds.h);
        if(child_moved){
            child->dirty=1;
            if(child->vtable->layout){
                child->layout_dirty=1;
            }
        }

        MGL_LOG_DBG(LOG_TAG,"child (%p) placed at (%d,%d,%d,%d)",
                    (void*)child,child_area.x,child_area.y,child_area.w,child_area.h);


        child=child->next_sibling;
    }

    if(fill_child){
        mgl_measure_constraint_t f_cw,f_ch;
        if(layout->direction==MGL_LINEAR_HORIZONTAL){
            f_cw.value=(mgl_coord_t)(content_w-cursor);
            f_cw.mode=MGL_MEASURE_EXACT;
            f_ch.value=content_h;
            f_ch.mode=(fill_child->pref_h==-2) ? MGL_MEASURE_EXACT : MGL_MEASURE_AT_MOST;
        }else{
            f_ch.value=(mgl_coord_t)(content_h-cursor);
            f_ch.mode=MGL_MEASURE_EXACT;
            f_cw.value=content_w;
            f_ch.mode=(fill_child->pref_h==-2) ? MGL_MEASURE_EXACT : MGL_MEASURE_AT_MOST;
        }

        mgl_coord_t f_w,f_h;
        fill_child->vtable->measure(fill_child,f_cw,f_ch,&f_w,&f_h);

        mgl_margin_t *m=&fill_child->margin;
        mgl_rect_t child_area;

        if(layout->direction==MGL_LINEAR_HORIZONTAL){
            mgl_coord_t offset=mgl_layout_align_offset_with_margin(
                    layout->cross_align,content_h,f_h,m->top,m->bottom);
            child_area.x=(mgl_coord_t)(content_x+cursor+m->left);
            child_area.y=(mgl_coord_t)(content_y+offset);
            child_area.w=f_w;
            child_area.h=f_h;
        }else{
            mgl_coord_t offset=mgl_layout_align_offset_with_margin(
                    layout->cross_align,content_w,f_w,m->left,m->right);
            child_area.x=(mgl_coord_t)(content_x+offset);
            child_area.y=(mgl_coord_t)(content_y+cursor+m->top);
            child_area.w=f_w;
            child_area.h=f_h;
        }

        mgl_rect_t old_bounds=fill_child->bounds;

        if(fill_child->vtable&&fill_child->vtable->layout){
            fill_child->prev_bounds=fill_child->bounds;
            fill_child->vtable->layout(fill_child,&child_area);
        }else{
            fill_child->prev_bounds=fill_child->bounds;
            fill_child->bounds=child_area;
        }

        bool child_moved=(fill_child->bounds.x!=old_bounds.x ||
                            fill_child->bounds.y!=old_bounds.y ||
                            fill_child->bounds.w!=old_bounds.w ||
                            fill_child->bounds.h!=old_bounds.h);
        if(child_moved){
            fill_child->dirty=1;
        }
    }

    MGL_LOG_DBG(LOG_TAG,"layout done (%p)",layout);

}

static const mgl_widget_vtable_t vtable={
        .draw=draw,
        .measure=linear_layout_measure,
        .layout=linear_layout_layout
};

void *mgl_linear_layout_init(void *memory,const void *args){
    const mgl_linear_layout_args_t *layout_args=(const mgl_linear_layout_args_t *)args;
    mgl_linear_layout_t *layout=(mgl_linear_layout_t *)memory;

    mgl_widget_init(&layout->base,&vtable,NULL,NULL,MGL_WIDGET_TYPE_LINEAR_LAYOUT);

    layout->direction=layout_args->direction;
    layout->padding_margin=layout_args->padding_margin;


    layout->cross_align=layout_args->cross_align;
    layout->main_align=layout_args->main_align;
    MGL_WIDGET_BASE_FIELD_HANDLE(layout,layout_args);
    MGL_WIDGET_PAINTER_FIELD_HANDLE(layout,layout_args);
    MGL_WIDGET_ROUND_RADIUS_FIELD_HANDLE(layout,layout_args);
    const mgl_widget_blueprint_t *const *child_bp=layout_args->children;
    if(child_bp){
        while(*child_bp){
            mgl_widget_t *child=mgl_blueprint_collect(*child_bp,&layout->base);
            if(child){
            }
            child_bp++;
        }
    }
    return &layout->base;
}

