#ifndef MGL_FB_H
#define MGL_FB_H

#include "core/mgl_types.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
    mgl_color_value_t *data; ///<缓冲区首地址
    mgl_coord_t width; ///<可见宽度
    mgl_coord_t height; ///<可见高度
    uint32_t stride_px; ///<每行实际像素数，≥ width（对齐填充用）
} mgl_fb_t;


void mgl_hal_set_fb(const mgl_fb_t *fb);
const mgl_color_value_t *mgl_hal_get_fb(void);

#ifdef __cplusplus
}
#endif
#endif //MGL_FB_H
