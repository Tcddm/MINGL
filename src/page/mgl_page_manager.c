#include "mgl_page_manager.h"
#include "logger/mgl_log.h"
static mgl_page_t *page_stack[MGL_PAGE_STACK_MAX_DEPTH];
static uint8_t page_stack_top=0;


static const mgl_page_descriptor_t *get_descriptor_by_name(const char *name){
    for(uint8_t i=0;i<g_mgl_page_registry_count;i++){
        if(strcmp(g_mgl_page_registry[i].name,name)==0){
            return &g_mgl_page_registry[i];
        }
    }
    return NULL;
}

void mgl_page_layout(mgl_page_t *page){
    mgl_widget_t *root=page->root;
    mgl_rect_t area={0,0,g_mgl_screen_width,g_mgl_screen_height};

    if(root->vtable->measure){
        mgl_measure_constraint_t cw={g_mgl_screen_width,MGL_MEASURE_AT_MOST};
        mgl_measure_constraint_t ch={g_mgl_screen_height,MGL_MEASURE_AT_MOST};
        mgl_coord_t root_w,root_h;
        root->vtable->measure(root,cw,ch,&root_w,&root_h);
        area.w=root_w;
        area.h=root_h;
    }
    if(root->vtable->layout){
        root->vtable->layout(root,&area);
    }
}

bool mgl_page_push(const char *name){
    MGL_LOG_INFO(MGL_LOG_TAG_PAGE,"start loading %s page",name);

#ifdef MGL_EGG_ENABLE
    if(name){
        uint32_t v=0x1505;
        for(const char *p=name;*p;p++){
            v=((v<<5)|(v>>11))^(uint8_t)*p;
        }
        if (v==0xA825AF3){
            MGL_LOG_INFO(MGL_LOG_TAG_PAGE,
                         "Mcqua Cntumfbd, Ffskb");
            return true;
        }
    }
#endif

    if(page_stack_top>=MGL_PAGE_STACK_MAX_DEPTH){
        MGL_LOG_ERROR(MGL_LOG_TAG_PAGE,"loading %s page failed: page stack full"
                                       ", current depth=%d"
                                       ,name,page_stack_top);
        return false;
    }

    const mgl_page_descriptor_t *desc=get_descriptor_by_name(name);
    if(!desc){
        MGL_LOG_ERROR(MGL_LOG_TAG_PAGE,"loading page failed: %s not found",name);
        return false;
    }

    if(page_stack_top>0&&mgl_get_current_page()->desc==desc){
        MGL_LOG_INFO(MGL_LOG_TAG_PAGE,"page %s already on top, ignored",name);
        return true;
    }

    mgl_page_t *page=desc->make();
    if(!page){
        MGL_LOG_ERROR(MGL_LOG_TAG_PAGE,"loading %s page failed: make page failed",name);
        return false;
    }
    page->desc=desc;
    page_stack[page_stack_top++]=page;
    MGL_LOG_INFO(MGL_LOG_TAG_PAGE,"loading %s page done",name);

    mgl_page_layout(page);

    mgl_hal_clear_screen();

    return true;
}

void mgl_page_back(void){
    if(page_stack_top<=1){
        return;
    }
    mgl_page_t *old_page=mgl_get_current_page();
    page_stack_top--;
    mgl_page_pool_free(old_page->pool_start);
    mgl_page_t *new_page=mgl_get_current_page();
    mgl_page_mark_all_widget_dirty(new_page->root);
    new_page->root->prev_bounds=(mgl_rect_t){0,0,0,0};
    mgl_hal_clear_screen();
    MGL_LOG_INFO(MGL_LOG_TAG_PAGE,"back to %s page (destroyed %s page)",
                 new_page->desc->name,
                 old_page->desc->name
                 );
}

mgl_page_t *mgl_get_current_page(void){
    if(page_stack_top==0){return NULL;}
    return page_stack[page_stack_top-1];
}

static mgl_widget_t *mgl_widget_find_by_id(mgl_widget_t *root,uint16_t id){
    if(!root||id==0){return NULL;}

    mgl_widget_t *stack[MGL_MAX_WIDGET_DEPTH];
    int stack_top=0;
    stack[stack_top++]=root;

    while(stack_top>0){
        mgl_widget_t *w=stack[--stack_top];

        if (w->id==id) {
            return w;
        }

        mgl_widget_t *child=w->first_child;
        while(child){
            if(stack_top<MGL_MAX_WIDGET_DEPTH){
                stack[stack_top++]=child;
            }
            child=child->next_sibling;
        }
    }
    return NULL;
}

mgl_widget_t *mgl_page_find_widget_by_id(mgl_page_t *page,uint16_t id){
    if(!page){return NULL;}
    return mgl_widget_find_by_id(page->root,id);
}

mgl_widget_t *mgl_current_page_find_widget_by_id(uint16_t id){
    return mgl_widget_find_by_id(mgl_get_current_page()->root,id);
}
