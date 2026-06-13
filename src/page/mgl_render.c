#include "mgl_render.h"
#include "logger/mgl_log.h"

/**
 * @brief 迭代遍历脏但未移动的容器子树，收集所有真正产生像素变化的叶子控件或移动容器的 prev∪bounds 区域
 *
 * @param container 脏但未移动的容器
 * @param rects 脏矩形结果数组
 * @param start_idx rects中已有条目数（非零表示前方已有脏矩形）
 * @return rects中的总条目数
 * @note 栈满时退化为收集当前子控件的全prev∪bounds
 */
static uint8_t render_collect_sub_dirty(mgl_widget_t *container,
                                        mgl_rect_t rects[],
                                        uint8_t start_idx){
    mgl_widget_t *stack[MGL_MAX_WIDGET_DEPTH];
    int sp=0;
    uint8_t count=start_idx;

    stack[sp++]=container;
    while(sp>0&&count<MGL_DIRTY_RECT_MAX_COUNT){
        mgl_widget_t *node=stack[--sp];

        for(mgl_widget_t *c=node->first_child;c;c=c->next_sibling){
            if(!c->dirty){continue;}

            bool changed=(c->prev_bounds.x!=c->bounds.x||
                            c->prev_bounds.y!=c->bounds.y||
                            c->prev_bounds.w!=c->bounds.w||
                            c->prev_bounds.h!=c->bounds.h);
            bool is_container=(c->vtable->layout!=NULL);

            if(!is_container||changed){
                //叶子控件或自身移动过的容器直接收集
                if(count<MGL_DIRTY_RECT_MAX_COUNT){
                    mgl_rect_union(&c->prev_bounds,&c->bounds,&rects[count]);
                    count++;
                }else{
                    mgl_rect_union(&rects[MGL_DIRTY_RECT_MAX_COUNT-1],
                                   &c->prev_bounds,
                                   &rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
                }
            }else{
                //脏但未移动的容器入栈，继续向下展开
                if(sp<MGL_MAX_WIDGET_DEPTH){
                    stack[sp++]=c;
                }else{
                    //栈深度耗尽退化为收集该容器的全区域
                    if(count<MGL_DIRTY_RECT_MAX_COUNT){
                        mgl_rect_union(&c->prev_bounds,&c->bounds,&rects[count]);
                        count++;
                    }else{
                        mgl_rect_union(&rects[MGL_DIRTY_RECT_MAX_COUNT-1],
                                       &c->prev_bounds,
                                       &rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
                    }
                }
            }
        }
    }
    return count;
}

/**
 * 收集指定控件的局部脏矩形数组
 * @param w 控件
 * @param rects 脏矩形结果数组
 * @return rects中的脏矩形条目数
 */
static uint8_t render_gather_dirty_rects(mgl_widget_t *w,mgl_rect_t rects[]){
    uint8_t count=0;
    bool has_layout=(w->vtable->layout!=NULL);

    //遍历脏子控件，收集脏矩形
    for(mgl_widget_t *c=w->first_child;c;c=c->next_sibling){
        if(!c->dirty){
            continue;
        }
        bool child_self_changed=(c->prev_bounds.x!=c->bounds.x||
                                   c->prev_bounds.y!=c->bounds.y||
                                   c->prev_bounds.w!=c->bounds.w||
                                   c->prev_bounds.h!=c->bounds.h);
        bool child_is_container=(c->vtable->layout!=NULL);
        if(child_is_container&&!child_self_changed){
            //容器仅因子控件冒泡变脏，展开子树取真实脏矩形
            count=render_collect_sub_dirty(c,rects,count);
        }else{
            //叶子控件或自身移动过的容器取其prev∪bounds
            if(count<MGL_DIRTY_RECT_MAX_COUNT){
                mgl_rect_union(&c->prev_bounds,&c->bounds,&rects[count]);
                count++;
            }else{
                mgl_rect_t rect;
                mgl_rect_union(&c->prev_bounds,&c->bounds,&rect);
                mgl_rect_union(&rects[MGL_DIRTY_RECT_MAX_COUNT-1],&rect,
                               &rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
            }
        }
    }

    //判断是否需要合并自身区域
    bool had_dirty_child=(count>0);
    bool self_changed=(w->prev_bounds.x!=w->bounds.x||
                         w->prev_bounds.y!=w->bounds.y||
                         w->prev_bounds.w!=w->bounds.w||
                         w->prev_bounds.h!=w->bounds.h);

    bool include_self=w->dirty;
    if(has_layout){
        include_self=w->dirty&&(self_changed||!had_dirty_child);
    }

    if(include_self){
        if(count<MGL_DIRTY_RECT_MAX_COUNT){
            mgl_rect_union(&w->prev_bounds,&w->bounds,&rects[count]);
            count++;
        }else{
            mgl_rect_union(&rects[MGL_DIRTY_RECT_MAX_COUNT-1],
                           &w->prev_bounds,&rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
            mgl_rect_union(&rects[MGL_DIRTY_RECT_MAX_COUNT-1],
                           &w->bounds,&rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
        }
    }

    return count;
}



void mgl_render_widget(mgl_widget_t *root,const mgl_rect_t *screen_clip){
    if(!root||!screen_clip){return;}

    //深度栈：每层保存（裁剪区，清空波及区，波及是否有效）
    mgl_rect_t clip_stack[MGL_MAX_WIDGET_DEPTH];
    mgl_rect_t clr_stack[MGL_MAX_WIDGET_DEPTH];
    bool clr_valid[MGL_MAX_WIDGET_DEPTH];
    int depth=0;

    mgl_widget_t *w=root;
    mgl_rect_t cur_clip=*screen_clip;
    mgl_rect_t cur_clear;
    bool has_clear=false;

    while(w){
        //不可见则跳过整个子树
        if(!w->visible){goto next;}

        //需要重排则调用 layout
        uint8_t was_layout_dirty=w->layout_dirty;
        if(was_layout_dirty&&w->vtable->layout){
            MGL_LOG_DBG(MGL_LOG_TAG_RENDER,"layout widget(%p): bounds=(%d,%d,%d,%d)",
                        (void*)w,
                        w->bounds.x, w->bounds.y, w->bounds.w, w->bounds.h);
            w->vtable->layout(w,&w->bounds);
            w->layout_dirty=0;
        }

        //收集局部脏矩形
        mgl_rect_t dirty_rects[MGL_DIRTY_RECT_MAX_COUNT];
        uint8_t dirty_count=render_gather_dirty_rects(w,dirty_rects);

        //逐矩形绘制和波及重绘
        mgl_rect_t merged_clear=cur_clear;
        bool has_merged_clear=has_clear;
        if(dirty_count>0){
            //逐脏矩形与父裁剪区求交后绘制
            for(uint8_t i=0;i<dirty_count;i++){
                mgl_rect_t draw_area;
                if(!mgl_rect_intersect(&dirty_rects[i],&cur_clip,&draw_area)){
                    continue;
                }

                if(w->vtable->draw){
                    MGL_LOG_DBG(MGL_LOG_TAG_RENDER,
                                "draw widget(%p): clip=(%d,%d,%d,%d) DIRTY (#%d/%d)",
                                (void*)w,draw_area.x,draw_area.y,draw_area.w,draw_area.h,
                                i+1,dirty_count);
                    mgl_draw_ctx_t ctx;
                    mgl_ctx_init(&ctx,w,&draw_area);
                    w->vtable->draw(&ctx);
                }
                //累积到 merged_clear,子控件需要感知本层全部绘制区域
                if(has_merged_clear){
                    mgl_rect_union(&merged_clear, &draw_area, &merged_clear);
                }else{
                    merged_clear=draw_area;
                    has_merged_clear=true;
                }
            }
            w->dirty = 0;
        }else if(has_clear){
            //不脏但被祖先波及区覆盖要求强制重绘
            mgl_rect_t draw_area;
            if(mgl_rect_intersect(&w->bounds,&cur_clear,&draw_area)){
                if(w->vtable->draw){
                    MGL_LOG_DBG(MGL_LOG_TAG_RENDER,
                                "draw widget(%p): clip=(%d,%d,%d,%d) CLEARED",
                                (void*)w,draw_area.x,draw_area.y,draw_area.w,draw_area.h);
                    mgl_draw_ctx_t ctx;
                    mgl_ctx_init(&ctx,w,&draw_area);
                    w->vtable->draw(&ctx);
                }
                if(has_merged_clear){
                    mgl_rect_union(&merged_clear,&draw_area,&merged_clear);
                }else{
                    merged_clear=draw_area;
                    has_merged_clear=true;
                }
            }
            w->dirty=0;
        }

        //子控件波及清空区（本层所有脏矩形绘制区域的合并结果）
        mgl_rect_t next_clear=merged_clear;
        bool next_has_clear=has_merged_clear;

        //递归子控件
        if(w->first_child){
            if(depth>=MGL_MAX_WIDGET_DEPTH){
                MGL_LOG_ERROR(MGL_LOG_TAG_RENDER,"depth overflow at widget(%p), subtree skipped",
                              (void*)w);
                goto next;
            }
            clip_stack[depth]=cur_clip;
            clr_stack[depth]=cur_clear;
            clr_valid[depth]=has_clear;
            depth++;

            mgl_rect_t child_area;
            mgl_rect_intersect(&w->bounds,&cur_clip,&child_area);
            cur_clip=child_area;

            cur_clear=next_clear;
            has_clear=next_has_clear;
            w=w->first_child;
            continue;
        }

        //本控件处理完毕，清理状态
        w->dirty=0;
        w->prev_bounds=w->bounds;

        next:
        //寻找下一兄弟，若无则回溯至父控件
        while(w&&!w->next_sibling){
            w=w->parent;
            if(w&&depth>0){
                depth--;
                cur_clip=clip_stack[depth];
                cur_clear=clr_stack[depth];
                has_clear=clr_valid[depth];
            }
            if(w){
                w->dirty=0;
                w->prev_bounds=w->bounds;
            }
        }
        if(w){w=w->next_sibling;}
    }
}

void mgl_render_page(mgl_page_t *page,mgl_rect_t screen){
    if (!page||!page->root||!page->root->dirty){return;}
    uint32_t start=mgl_hal_get_tick_ms();
    MGL_LOG_DBG(MGL_LOG_TAG_RENDER,"render start");
    mgl_render_widget(page->root,&screen);
    MGL_LOG_DBG(MGL_LOG_TAG_RENDER,"render done (%ums)",mgl_hal_get_tick_ms()-start);
}