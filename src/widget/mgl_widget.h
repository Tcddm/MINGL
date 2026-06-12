#ifndef MGL_WIDGET_H
#define MGL_WIDGET_H

#include "core/mgl_types.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef bool (*mgl_action_handler_t)(mgl_widget_t *,const mgl_action_event_t *);

#define MGL_WIDGET_ACTION_HANDLE_ACTION(custom_widget,self,event,handled) \
    do{ \
        mgl_action_type_t __act=mgl_widget_get_action(self,event); \
        if (__act!=MGL_ACTION_NONE&&(custom_widget)->action_handler){ \
            mgl_action_event_t __ae={.type= __act,.widget=self}; \
            (handled)=(custom_widget)->action_handler(self,&__ae); \
        } \
    } while(0)

#define MGL_WIDGET_ACTION_HANDLER \
    mgl_action_handler_t action_handler;

#define MGL_WIDGET_BASE_ARGS_FIELD \
    mgl_margin_t margin; \
    mgl_coord_t pref_w; \
    mgl_coord_t pref_h; \
    uint16_t id;


#define MGL_WIDGET_BASE_FIELD_HANDLE(custom_widget,custom_widget_args) \
    do{ \
        custom_widget->base.margin=custom_widget_args->margin;         \
        if(custom_widget_args->pref_w==0){ \
            custom_widget->base.pref_w=-1;                                                                         \
        }else{ \
            custom_widget->base.pref_w=custom_widget_args->pref_w;                                                                         \
        } \
        if(custom_widget_args->pref_h==0){ \
            custom_widget->base.pref_h=-1;                                                                       \
        }else{ \
            custom_widget->base.pref_h=custom_widget_args->pref_h;                                                                   \
        } \
        if(custom_widget_args->id!=0){ \
            custom_widget->base.id=custom_widget_args->id; \
        } \
    }while(0)


#define MGL_WIDGET_FIELD_HANDLE(custom_widget,custom_widget_args,name) \
    do{ \
        custom_widget->name=custom_widget_args->name; \
    }while(0)

#define MGL_WIDGET_ROUND_RADIUS_FIELD \
    mgl_coord_t round_radius;

#define MGL_WIDGET_ROUND_RADIUS_FIELD_HANDLE(custom_widget,custom_widget_args) \
    MGL_WIDGET_FIELD_HANDLE(custom_widget,custom_widget_args,round_radius)

#define MGL_WIDGET_ACTION_HANDLER_FIELD_HANDLE(custom_widget,custom_widget_args) \
    MGL_WIDGET_FIELD_HANDLE(custom_widget,custom_widget_args,action_handler)

#define MGL_MEASURE_RESOLVE(pref_size, natural_size, constraint, out_size) \
    do { \
        mgl_coord_t __base; \
        if((pref_size)!=-1){ \
            __base=(pref_size); \
        }else{ \
            __base=(natural_size); \
        } \
        if((constraint).mode==MGL_MEASURE_EXACT) { \
            *(out_size)=(constraint).value; \
        }else if((constraint).mode==MGL_MEASURE_AT_MOST){ \
            if(__base<(constraint).value) { \
                *(out_size)=__base; \
            }else{ \
                *(out_size)=(constraint).value; \
            } \
        }else{ \
            *(out_size)=__base; \
        } \
    }while(0)

typedef enum{
    MGL_MEASURE_EXACT,
    MGL_MEASURE_AT_MOST,
    MGL_MEASURE_NONE
} mgl_measure_mode_t;

typedef struct {
    mgl_coord_t value;
    uint8_t mode;
} mgl_measure_constraint_t;

typedef struct {
    void (*draw)(mgl_draw_ctx_t *ctx);
    bool (*on_event)(mgl_widget_t *self,const mgl_event_t *event);
    void (*measure)(mgl_widget_t *self,
                    mgl_measure_constraint_t constraint_w,
                    mgl_measure_constraint_t constraint_h,
                    mgl_coord_t *out_w,
                    mgl_coord_t *out_h);
    void (*layout)(mgl_widget_t *self,
                   const mgl_rect_t *area);
} mgl_widget_vtable_t;

struct mgl_widget_t{
    const mgl_widget_vtable_t *vtable;
    uint16_t id;
    uint16_t type;

    mgl_rect_t bounds;
    mgl_rect_t prev_bounds;

    mgl_margin_t margin;
    mgl_coord_t pref_w;
    mgl_coord_t pref_h;

    mgl_widget_t *parent;                //父控件
    mgl_widget_t *first_child;           //第一个子控件
    mgl_widget_t *next_sibling;          //后一个兄弟

    uint8_t dirty:1;
    uint8_t layout_dirty:1;
    uint8_t visible:1;
    uint8_t enabled:1;
    uint8_t focused:1;
    uint8_t reserved:3;

    int8_t  anim_slot;

    void *user_data;
    const char *name;

};

typedef enum{
    MGL_WIDGET_TYPE_LINEAR_LAYOUT=0,
    MGL_WIDGET_TYPE_LABEL,
    MGL_WIDGET_TYPE_BUTTON
} mgl_widget_type;

void mgl_widget_init(mgl_widget_t *widget,const mgl_widget_vtable_t *vtable,const char *name,void *user_data,uint16_t type);
void mgl_widget_add_child(mgl_widget_t *parent,mgl_widget_t *child);
mgl_action_type_t mgl_widget_get_action(mgl_widget_t *self,const mgl_event_t *event);

static inline void mgl_widget_set_dirty(mgl_widget_t *w) {
    while(w){
        w->dirty=1;
        w=w->parent;
    }
}

static inline void mgl_widget_set_dirty_content(mgl_widget_t *w){
    if(w->vtable->measure&&w->parent&&w->parent->vtable->layout){
        mgl_coord_t nw,nh;
        w->vtable->measure(w,
                           (mgl_measure_constraint_t){32767,MGL_MEASURE_NONE},
                           (mgl_measure_constraint_t){32767,MGL_MEASURE_NONE},
                           &nw,&nh);
        if(nw!=w->bounds.w||nh!=w->bounds.h){
            w->parent->layout_dirty=1;
        }
    }
    mgl_widget_set_dirty(w);
}

#ifdef __cplusplus
}
#endif
#endif //MGL_WIDGET_H
