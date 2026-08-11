#include "hal_sdl.h"
#include <stdio.h>
#include <SDL.h>
#include "core/mgl_core.h"
#include "page/mgl_page_manager.h"

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
    while(!quit){
        mgl_hal_sdl2_poll_events(&quit);
        mgl_core_tick();

        while (mgl_hal_get_tick_ms()-last_tick<FRAME_MS){
            mgl_hal_sdl2_poll_events(&quit);
            if(quit){break;}
        }
        last_tick=mgl_hal_get_tick_ms();
    }

    mgl_hal_sdl2_cleanup();
    return 0;
}