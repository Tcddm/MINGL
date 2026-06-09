#include "mgl_render.h"
void mgl_render_widget(mgl_widget_t *root, const mgl_rect_t *screen_clip) {
    if (!root || !screen_clip) return;

    mgl_rect_t clip_stack[MGL_MAX_WIDGET_DEPTH];
    int depth = 0;
    mgl_widget_t *w = root;
    mgl_rect_t current_clip = *screen_clip;

    while (w) {
        //不可见控件跳过
        if (!w->visible) goto next;

        //计算本控件绘制区域
        mgl_rect_t draw_area;
        if (mgl_rect_intersect(&w->bounds, &current_clip, &draw_area)) {
            if (w->dirty && w->vtable->draw) {
                mgl_draw_ctx_t ctx;
                mgl_ctx_init(&ctx, w, &draw_area);
                w->vtable->draw(&ctx);
                w->dirty = false;
            }

            //有子控件则下降
            if (w->first_child) {
                if (depth >= MGL_MAX_WIDGET_DEPTH) {
                    // 深度溢出，跳过该子树，继续兄弟
                    goto next;
                }
                //保存当前（父）裁剪区域，更新为子控件裁剪区
                clip_stack[depth] = current_clip;
                depth++;
                current_clip = draw_area;
                w = w->first_child;
                continue;
            }
        }

        next:
        //寻找下一个兄弟，若无则回溯
        while (w && !w->next_sibling) {
            w = w->parent;
            if (w && depth > 0) {
                depth--;
                current_clip = clip_stack[depth];
            }
        }
        if (w) {
            w = w->next_sibling;   //移动至兄弟，current_clip仍为父裁剪区
        }
    }
}

void mgl_render_page(mgl_page_t *page,mgl_rect_t screen){
    if (!page || !page->root) return;
    mgl_hal_clear_screen();
    mgl_page_mark_all_widget_dirty(page->root);
    mgl_render_widget(page->root, &screen);
}