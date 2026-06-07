#include "mgl_core.h"
#include "logger/mgl_log.h"
mgl_coord_t g_mgl_screen_height;
mgl_coord_t g_mgl_screen_width;
void mgl_core_init(void){
    MGL_LOG_INFO(MGL_LOG_TAG_CORE,"MINGL start init");
    mgl_core_set_screen_size(MGL_SCREEN_WIDTH,MGL_SCREEN_HEIGHT);
    MGL_LOG_INFO(MGL_LOG_TAG_CORE,"MINGL init done");
}
void mgl_core_set_screen_size(mgl_coord_t width,mgl_coord_t height){
    g_mgl_screen_width=width;
    g_mgl_screen_height=height;
    mgl_draw_update_screen_size(g_mgl_screen_width, g_mgl_screen_height);
}
