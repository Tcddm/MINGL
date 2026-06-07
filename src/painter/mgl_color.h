#ifndef MGL_COLOR_H
#define MGL_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif



#define MGL_RGB565_RAW(r,g,b)  ((((r)&0xF8)<<8)|(((g)&0xFC)<<3)|((b)>>3))

#define MGL_COLOR_BLACK        ((mgl_color_t){.value=0x0000,.alpha=255 })
#define MGL_COLOR_WHITE        ((mgl_color_t){.value=0xFFFF,.alpha=255 })
#define MGL_COLOR_RED          ((mgl_color_t){.value=0xF800,.alpha=255 })
#define MGL_COLOR_GREEN        ((mgl_color_t){.value=0x07E0,.alpha=255 })
#define MGL_COLOR_BLUE         ((mgl_color_t){.value=0x001F,.alpha=255 })


#define MGL_COLOR_TRANSPARENT  ((mgl_color_t){.value=0x0000,.alpha=0})

#define MGL_COLOR(r,g,b)       ((mgl_color_t){.value=MGL_RGB565_RAW(r,g,b),.alpha=255})

#ifdef __cplusplus
}
#endif

#endif // MGL_COLOR_H