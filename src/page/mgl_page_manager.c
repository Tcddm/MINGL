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

    return true;
}

mgl_page_t *mgl_get_current_page(void){
    if(page_stack_top==0){return NULL;}
    return page_stack[page_stack_top-1];
}

