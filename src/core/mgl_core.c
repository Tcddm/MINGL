#include "mgl_core.h"
#include "logger/mgl_log.h"
#include "page/mgl_page_manager.h"
#include "event/mgl_event.h"
#include "page/mgl_render.h"
#include "timer/mgl_timer.h"

mgl_coord_t g_mgl_screen_height;
mgl_coord_t g_mgl_screen_width;
void mgl_core_init(void){
    MGL_LOG_INFO(MGL_LOG_TAG_CORE,"MINGL %s start init (built %s %s)",MGL_VERSION_STRING,__DATE__,__TIME__);
    mgl_core_set_screen_size(MGL_SCREEN_WIDTH,MGL_SCREEN_HEIGHT);
    mgl_page_init_overlay();
    MGL_LOG_INFO(MGL_LOG_TAG_CORE,"MINGL init done");
}
void mgl_core_set_screen_size(mgl_coord_t width,mgl_coord_t height){
    g_mgl_screen_width=width;
    g_mgl_screen_height=height;
    mgl_draw_update_screen_size(g_mgl_screen_width, g_mgl_screen_height);
}

void mgl_core_tick(void){
    mgl_widget_t *root=mgl_get_current_page()->root;

    mgl_touch_data_t touch;
    if(mgl_hal_get_touch(&touch)){
        mgl_process_touch_data(&touch,root);
    }else{
        mgl_process_touch_data(NULL,root);
    }

    mgl_timer_tick();

    mgl_render_page(mgl_get_current_page(),g_mgl_full_screen_ctx.clip);
}
