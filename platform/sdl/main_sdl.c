#include "hal_sdl.h"
#include <stdio.h>
#include <SDL.h>
#include "core/mgl_core.h"
#include "page/mgl_render.h"
#include "page/mgl_page_manager.h"
#include "event/mgl_event.h"

#if MGL_DEMO_DEV
#include "main_page.h"
#include "count/count_page.h"
#include "list_test/list_page.h"
#endif
#if MGL_DEMO_DEV
MGL_PAGE_REGISTRY_START()

    MGL_PAGE_ENTRY("main",make_demo_dev_main_page),
    MGL_PAGE_ENTRY("count",make_demo_dev_count_page),
    MGL_PAGE_ENTRY("list_test",make_demo_dev_list_page),

MGL_PAGE_REGISTRY_END()
#endif

#define FRAME_MS 16

int main(int argc,char *argv[]) {
    setvbuf(stdout,NULL,_IONBF,0);
    
    mgl_core_init();
    
    if (!mgl_hal_sdl2_init("MINGL Simulator")) {
        fprintf(stderr,"Failed to initialize SDL2.\n");
        return EXIT_FAILURE;
    }
    
    bool quit=false;
    uint32_t last_tick=mgl_hal_get_tick_ms();

    mgl_page_push("main");
    mgl_hal_clear_screen();
    while(!quit){
        mgl_hal_sdl2_poll_events(&quit);

        mgl_render_page(mgl_get_current_page(),g_mgl_full_screen_ctx.clip);

        mgl_touch_data_t touch;
        if(mgl_hal_get_touch(&touch)){
            mgl_process_touch_data(&touch,mgl_get_current_page()->root);
        }else{
            mgl_process_touch_data(NULL,mgl_get_current_page()->root);
        }

        mgl_hal_flush_display();

        while (mgl_hal_get_tick_ms()-last_tick<FRAME_MS){
            mgl_hal_sdl2_poll_events(&quit);
            if(quit){break;}
        }
        last_tick=mgl_hal_get_tick_ms();
    }

    mgl_hal_sdl2_cleanup();
    return 0;
}