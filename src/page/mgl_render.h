#ifndef MGL_RENDER_H
#define MGL_RENDER_H

#include "draw/mgl_draw_ctx.h"
#include "mgl_page.h"

#ifdef __cplusplus
extern "C"{
#endif

void mgl_render_page(mgl_page_t *page,mgl_rect_t screen);
void mgl_render_widget(mgl_widget_t *root,const mgl_rect_t *screen_clip);

#ifdef __cplusplus
}
#endif
#endif //MGL_RENDER_H
