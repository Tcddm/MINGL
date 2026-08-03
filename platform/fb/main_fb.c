#include "hal_fb.h"
#include <stdio.h>
#include <stdlib.h>
#include "core/mgl_core.h"
#include "page/mgl_render.h"
#include "page/mgl_page_manager.h"
#include "event/mgl_event.h"

#define FRAME_MS 16

int main(int argc, char *argv[]){
    (void)argc;
    (void)argv;

    mgl_core_init();

    if(!mgl_hal_fb_init(MGL_PLATFORM_FB_FB_DEVICE)){
        fprintf(stderr,"Failed to initialize framebuffer\n");
        return EXIT_FAILURE;
    }

    mgl_page_push("main");
    mgl_hal_clear_screen();

    uint32_t last_tick=mgl_hal_get_tick_ms();
    while(1){
        mgl_render_page(mgl_get_current_page(),g_mgl_full_screen_ctx.clip);

        mgl_touch_data_t touch;
        if(mgl_hal_get_touch(&touch)){
            mgl_process_touch_data(&touch,mgl_get_current_page()->root);
        }else{
            mgl_process_touch_data(NULL,mgl_get_current_page()->root);
        }

        while(mgl_hal_get_tick_ms()-last_tick<FRAME_MS);
        last_tick=mgl_hal_get_tick_ms();
    }

    return 0;
}
