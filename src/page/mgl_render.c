#include "mgl_render.h"
#include "logger/mgl_log.h"
#include "mgl_page_manager.h"
#if MGL_FPS_LOG
static uint32_t render_frames=0;
static uint32_t skip_frames=0;
static uint32_t last_report=0;
static uint32_t render_total_ms=0;
static uint32_t render_max_ms=0;
static uint32_t render_min_ms=UINT32_MAX;
#endif
#if MGL_LOG_ENABLE_RENDER_DETAIL
#define DETAIL_LOG(tag,fmt,...) MGL_LOG_DBG(tag,fmt,##__VA_ARGS__)
#else
#define DETAIL_LOG(tag,fmt,...) ((void)0)
#endif
/**
 * @brief 将rect加入脏矩形数组，溢出时合并到最后一条
 *
 * @param count 当前已有条数
 * @return 新条数
 */
static inline uint8_t render_add_dirty_rect(mgl_rect_t rects[],uint8_t count,
                                            const mgl_rect_t *rect){
    if(count<MGL_DIRTY_RECT_MAX_COUNT){
        rects[count++]=*rect;
    }else{
        mgl_rect_union(&rects[MGL_DIRTY_RECT_MAX_COUNT-1],rect,
                       &rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
    }
    return count;
}
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
                mgl_rect_t d;
                mgl_rect_union(&c->prev_bounds,&c->bounds,&d);
                count=render_add_dirty_rect(rects,count,&d);
            }else{
                //脏但未移动的容器入栈，继续向下展开
                if(sp<MGL_MAX_WIDGET_DEPTH){
                    stack[sp++]=c;
                }else{
                    //栈深度耗尽退化为收集该容器的全区域
                    mgl_rect_t d;
                    mgl_rect_union(&c->prev_bounds,&c->bounds,&d);
                    count=render_add_dirty_rect(rects,count,&d);
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
        if(child_is_container&&!child_self_changed&&!c->force_redraw){
            //容器仅因子控件冒泡变脏，展开子树取真实脏矩形
            count=render_collect_sub_dirty(c,rects,count);
        }else{
            //叶子控件或自身移动过的容器取其prev∪bounds
            mgl_rect_t d;
            mgl_rect_union(&c->prev_bounds,&c->bounds,&d);
            count=render_add_dirty_rect(rects,count,&d);
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
        include_self=w->dirty&&(self_changed||!had_dirty_child||w->force_redraw);
    }

    if(include_self){
        mgl_rect_t sr;
        mgl_rect_union(&w->prev_bounds,&w->bounds,&sr);
        uint8_t j=0;
        for(uint8_t i=0;i<count;i++){
            if(rects[i].x<sr.x || rects[i].y<sr.y ||
               rects[i].x+rects[i].w>sr.x+sr.w ||
               rects[i].y+rects[i].h>sr.y+sr.h){
                if(j!=i){rects[j]=rects[i];}
                j++;
            }
        }
        count=j;
        count=render_add_dirty_rect(rects,count,&sr);
    }

    return count;
}



void mgl_render_widget(mgl_widget_t *root,const mgl_rect_t *screen_clip,mgl_rect_t *flush_rects,uint8_t *flush_count){
    if(!root||!screen_clip){return;}

    //深度栈：每层保存（裁剪区，清空波及区数组）
    mgl_rect_t clip_stack[MGL_MAX_WIDGET_DEPTH];
    mgl_rect_t clr_stack[MGL_MAX_WIDGET_DEPTH][MGL_DIRTY_RECT_MAX_COUNT];
    uint8_t clr_count_stack[MGL_MAX_WIDGET_DEPTH];
    int depth=0;

    mgl_widget_t *w=root;
    mgl_rect_t cur_clip=*screen_clip;
    mgl_rect_t cur_clear[MGL_DIRTY_RECT_MAX_COUNT];
    uint8_t cur_clear_count=0;

    while(w){
        // #region mgl_render_widget_step1
        //不可见则跳过整个子树
        if(w->hidden){goto next;}
        // #endregion

        // #region mgl_render_widget_step2
        //需要重排则调用 layout
        uint8_t was_layout_dirty=w->layout_dirty;
        if(was_layout_dirty&&w->vtable->layout){
            MGL_LOG_DBG(MGL_LOG_TAG_RENDER,"layout widget(%p): bounds=(%d,%d,%d,%d)",
                        (void*)w,
                        w->bounds.x, w->bounds.y, w->bounds.w, w->bounds.h);
            w->vtable->layout(w,&w->bounds);
            w->layout_dirty=0;
        }
        // #endregion

        // #region mgl_render_widget_step3
        //收集局部脏矩形
        mgl_rect_t dirty_rects[MGL_DIRTY_RECT_MAX_COUNT];
        uint8_t dirty_count=render_gather_dirty_rects(w,dirty_rects);
        // #endregion

        // #region mgl_render_widget_step4
        //逐矩形绘制和波及重绘
        mgl_rect_t clear_rects[MGL_DIRTY_RECT_MAX_COUNT];
        uint8_t clear_count=0;

        //继承父控件的波及区
        for(uint8_t i=0;i<cur_clear_count;i++){
            clear_rects[clear_count++]=cur_clear[i];
        }

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

                //不合并，独立追加到波及区
                if(clear_count<MGL_DIRTY_RECT_MAX_COUNT){
                    clear_rects[clear_count++]=draw_area;
                }else{
                    mgl_rect_union(&clear_rects[MGL_DIRTY_RECT_MAX_COUNT-1],&draw_area,
                                   &clear_rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
                }
                if(*flush_count<MGL_DIRTY_RECT_MAX_COUNT){
                    flush_rects[(*flush_count)++]=draw_area;
                }else{
                    mgl_rect_union(&flush_rects[MGL_DIRTY_RECT_MAX_COUNT-1],&draw_area,
                                   &flush_rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
                }
            }
            w->dirty=0;
        }else if(cur_clear_count>0){
            //不脏但被祖先波及区覆盖要求强制重绘
            for(uint8_t i=0;i<cur_clear_count;i++){
                mgl_rect_t draw_area;
                if(!mgl_rect_intersect(&w->bounds,&cur_clear[i],&draw_area)){
                    continue;
                }
                if(w->vtable->draw){
                    MGL_LOG_DBG(MGL_LOG_TAG_RENDER,
                                "draw widget(%p): clip=(%d,%d,%d,%d) CLEARED (#%d/%d)",
                                (void*)w,draw_area.x,draw_area.y,draw_area.w,draw_area.h,
                                i+1,cur_clear_count);
                    mgl_draw_ctx_t ctx;
                    mgl_ctx_init(&ctx,w,&draw_area);
                    w->vtable->draw(&ctx);
                }
                if(clear_count<MGL_DIRTY_RECT_MAX_COUNT){
                    clear_rects[clear_count++]=draw_area;
                }else{
                    mgl_rect_union(&clear_rects[MGL_DIRTY_RECT_MAX_COUNT-1],&draw_area,
                                   &clear_rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
                }
                if(*flush_count<MGL_DIRTY_RECT_MAX_COUNT){
                    flush_rects[(*flush_count)++]=draw_area;
                }else{
                    mgl_rect_union(&flush_rects[MGL_DIRTY_RECT_MAX_COUNT-1],&draw_area,
                                   &flush_rects[MGL_DIRTY_RECT_MAX_COUNT-1]);
                }
            }
            w->dirty=0;
        }
        // #endregion

        // #region mgl_render_widget_step5
        //子控件波及清空区（将本层波及区逐条和 bounds 求交传给子控件）
        mgl_rect_t next_clear[MGL_DIRTY_RECT_MAX_COUNT];
        uint8_t next_clear_count=0;
        for(uint8_t i=0;i<clear_count;i++){
            mgl_rect_t nc;
            if(mgl_rect_intersect(&clear_rects[i],&w->bounds,&nc)){
                if(next_clear_count<MGL_DIRTY_RECT_MAX_COUNT){
                    next_clear[next_clear_count++]=nc;
                }else{
                    mgl_rect_union(&next_clear[MGL_DIRTY_RECT_MAX_COUNT-1],&nc,
                                   &next_clear[MGL_DIRTY_RECT_MAX_COUNT-1]);
                }
            }
        }
        // #endregion

        // #region mgl_render_widget_step6
        //递归子控件
        if(w->first_child){
            if(depth>=MGL_MAX_WIDGET_DEPTH){
                MGL_LOG_ERROR(MGL_LOG_TAG_RENDER,"depth overflow at widget(%p), subtree skipped",
                              (void*)w);
                goto next;
            }
            clip_stack[depth]=cur_clip;
            for(uint8_t i=0;i<cur_clear_count;i++){
                clr_stack[depth][i]=cur_clear[i];
            }
            clr_count_stack[depth]=cur_clear_count;
            depth++;

            mgl_rect_t child_area;
            mgl_rect_intersect(&w->bounds,&cur_clip,&child_area);
            cur_clip=child_area;

            for(uint8_t i=0;i<next_clear_count;i++){
                cur_clear[i]=next_clear[i];
            }
            cur_clear_count=next_clear_count;
            w=w->first_child;
            continue;
        }
        // #endregion

        // #region mgl_render_widget_step7_1
        //本控件处理完毕，清理状态
        w->dirty=0;
        w->prev_bounds=w->bounds;
        // #endregion

        // #region mgl_render_widget_step7_2
        next:
        //寻找下一兄弟，若无则回溯至父控件
        while(w&&!w->next_sibling){
            w=w->parent;
            if(w&&depth>0){
                depth--;
                cur_clip=clip_stack[depth];
                for(uint8_t i=0;i<clr_count_stack[depth];i++){
                    cur_clear[i]=clr_stack[depth][i];
                }
                cur_clear_count=clr_count_stack[depth];
            }
            if(w){
                w->dirty=0;
                w->prev_bounds=w->bounds;
            }
        }
        if(w){w=w->next_sibling;}
        // #endregion
    }
}

static bool overlay_has_visible(mgl_widget_t *root){
    for(mgl_widget_t *c=root->first_child;c;c=c->next_sibling){
        if(!c->hidden){return true;}
    }
    return false;
}

void mgl_render_page(mgl_page_t *page,mgl_rect_t screen){
#if MGL_FPS_LOG
    uint32_t now=mgl_hal_get_tick_ms();
    if(now-last_report>=MGL_FPS_REPORT_INTERVAL_MS){
        uint32_t total=render_frames+skip_frames;
        uint32_t idle=total ? (skip_frames*100/total) : 0;
        MGL_LOG_INFO(MGL_LOG_TAG_RENDER,
                     "FPS render=%u skip=%u total=%u idle=%u%% avg=%ums min=%ums max=%ums",
                     render_frames,skip_frames,total,idle,
                     render_frames ? (render_total_ms/render_frames) : 0,
                     render_frames ? render_min_ms : 0,
                     render_max_ms);
        render_max_ms=0;
        render_min_ms=UINT32_MAX;
        render_total_ms=0;
        render_frames=0;
        skip_frames=0;
        last_report=now;
    }
#endif
    mgl_page_t *overlay_check=mgl_page_get_overlay();
    uint8_t overlay_render_needed=
            overlay_check&&overlay_check->root
            &&overlay_check->root->dirty
            &&overlay_has_visible(overlay_check->root);
    if(!page||!page->root||(!page->root->dirty&&!overlay_render_needed)){
#if MGL_FPS_LOG
    skip_frames++;
#endif
        return;
    }
#if MGL_FPS_LOG
    render_frames++;
#endif
    uint32_t start=mgl_hal_get_tick_ms();
    MGL_LOG_DBG(MGL_LOG_TAG_RENDER,"render start");
    mgl_rect_t flush_rects[MGL_DIRTY_RECT_MAX_COUNT];
    uint8_t flush_count=0;
    uint8_t page_was_dirty=page->root->dirty;
    mgl_render_widget(page->root,&screen,flush_rects,&flush_count);
    mgl_page_t *overlay=mgl_page_get_overlay();
    if(overlay&&overlay->root){
        if(overlay_has_visible(overlay->root)){
            if(page_was_dirty){
                mgl_widget_t *stack[MGL_MAX_WIDGET_DEPTH];
                int sp=0;
                stack[sp++]=overlay->root;
                while(sp>0){
                    mgl_widget_t *ow=stack[--sp];
                    ow->dirty=0;
                    for(mgl_widget_t *c=ow->first_child;c;c=c->next_sibling){
                        if(sp<MGL_MAX_WIDGET_DEPTH){stack[sp++]=c;}
                    }
                }
                mgl_widget_set_dirty_full(overlay->root);
            }
            mgl_render_widget(overlay->root,&screen,flush_rects,&flush_count);
        }else{
            overlay->root->dirty=0;
        }
    }
    mgl_hal_flush_display(flush_rects,flush_count);
    uint32_t time=mgl_hal_get_tick_ms()-start;
    MGL_LOG_DBG(MGL_LOG_TAG_RENDER,"render done (%ums)",time);
#if MGL_FPS_LOG
    if(time>render_max_ms){ render_max_ms=time;}
    if(time<render_min_ms){ render_min_ms=time;}
    render_total_ms+=time;
#endif
}