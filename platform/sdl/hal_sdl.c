#include "hal_sdl.h"
#include <SDL.h>
static SDL_Window *g_window=NULL;
static SDL_Renderer *g_renderer=NULL;
static uint32_t g_start_tick=0;
static bool g_touch_pressed=false;
static int16_t g_touch_x=0,g_touch_y=0;
bool mgl_hal_sdl2_init(const char *title){
    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        return false;
    }
    g_window=SDL_CreateWindow(title,
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                g_mgl_screen_width,g_mgl_screen_height,
                                SDL_WINDOW_SHOWN);
    if(!g_window){
        SDL_Quit();
        return false;
    }

    g_renderer=SDL_CreateRenderer(g_window,-1,
                                    SDL_RENDERER_ACCELERATED);
    if(!g_renderer){
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return false;
    }

    g_start_tick=SDL_GetTicks();
    return true;
}

void mgl_hal_sdl2_cleanup(void){
    if(g_renderer){SDL_DestroyRenderer(g_renderer);}
    if(g_window){SDL_DestroyWindow(g_window);}
    SDL_Quit();
}

void mgl_hal_sdl2_poll_events(bool *quit){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT:
                *quit=true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(e.button.button==SDL_BUTTON_LEFT){
                    g_touch_pressed=true;
                    g_touch_x=(int16_t)e.button.x;
                    g_touch_y=(int16_t)e.button.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if(e.button.button==SDL_BUTTON_LEFT){
                    g_touch_pressed=false;
                }
                break;
            case SDL_MOUSEMOTION:
                if(g_touch_pressed){
                    g_touch_x=(int16_t)e.motion.x;
                    g_touch_y=(int16_t)e.motion.y;
                }
                break;
            default:
                break;
        }
    }
}
static inline void rgb565_to_rgba(SDL_Renderer *r,uint16_t c,uint8_t a){
    if(a<128){return;}
    uint8_t red= (c >> 11) & 0x1F;
    red=(red << 3) | (red >> 2);
    uint8_t green=(c >> 5) & 0x3F;
    green=(green << 2) | (green >> 4);
    uint8_t blue=c & 0x1F;
    blue=(blue << 3) | (blue >> 2);
    SDL_SetRenderDrawColor(r,red,green,blue,255);
}
void mgl_hal_set_pixel(mgl_coord_t x,mgl_coord_t y,mgl_color_t color){
    if(color.alpha<128){ return;}
    rgb565_to_rgba(g_renderer,color.value,color.alpha);
    SDL_RenderDrawPoint(g_renderer,x,y);
}

uint32_t mgl_hal_get_tick_ms(void){
    return SDL_GetTicks()-g_start_tick;
}

bool mgl_hal_get_touch(mgl_touch_data_t *touch){
    touch->count=0;
    if(g_touch_pressed){
        touch->points[0].x=g_touch_x;
        touch->points[0].y=g_touch_y;
        touch->points[0].pressed=true;
        touch->points[0].finger_id=0;
        touch->count=1;
        return true;
    }
    return false;
}

void mgl_hal_flush_display(void){
    SDL_RenderPresent(g_renderer);
}

void mgl_hal_fill_rect(mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_color_t color){
    if(color.alpha<128){ return;}
    rgb565_to_rgba(g_renderer,color.value,color.alpha);
    SDL_Rect rect={x,y,w,h};
    SDL_RenderFillRect(g_renderer,&rect);
}

void mgl_hal_clear_screen(void){
    mgl_coord_t w=g_mgl_screen_width;
    mgl_coord_t h=g_mgl_screen_height;
    mgl_color_t a={0x0000,255};
    mgl_hal_fill_rect(0,0,w,h,a);
}
void mgl_hal_bit_blt(mgl_coord_t x,mgl_coord_t y,
                     const mgl_rect_t *src_rect,
                     const mgl_bitmap_t *bmp,
                     mgl_color_t transparent_color){
    if(!g_renderer||!bmp||!bmp->data){return;}

    SDL_Surface *surface=SDL_CreateRGBSurfaceWithFormat(
            0,bmp->w,bmp->h,16,SDL_PIXELFORMAT_RGB565);

    if(!surface){ return;}
    size_t row_size=bmp->w*2;
    for(int i=0;i<bmp->h;i++){
        memcpy((uint8_t*)surface->pixels+i*surface->pitch,
               bmp->data+i*row_size,row_size);
    }

    if(transparent_color.alpha==0&&transparent_color.value!=0xFFFF){
        uint8_t r=(transparent_color.value >> 11) & 0x1F;
        uint8_t g=(transparent_color.value >> 5) & 0x3F;
        uint8_t b=transparent_color.value & 0x1F;
        uint32_t key=SDL_MapRGB(surface->format,r<<3,g<<2,b<<3);
        SDL_SetColorKey(surface,SDL_TRUE,key);
    }
    
    SDL_Texture *texture=SDL_CreateTextureFromSurface(g_renderer,surface);
    SDL_FreeSurface(surface);
    if(!texture){ return;}
    
    SDL_Rect src={0,0,bmp->w,bmp->h};
    if(src_rect){
        src.x=src_rect->x;
        src.y=src_rect->y;
        src.w=src_rect->w;
        src.h=src_rect->h;
    }
    SDL_Rect dst={x,y,src.w,src.h};
    SDL_RenderCopy(g_renderer,texture,&src,&dst);
    SDL_DestroyTexture(texture);
}