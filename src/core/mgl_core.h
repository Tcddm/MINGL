#ifndef MGL_CORE_H
#define MGL_CORE_H
#include "hal/mgl_hal.h"
#include "draw/mgl_draw.h"
#ifdef __cplusplus
extern "C"{
#endif
void mgl_core_init(void);
void mgl_core_set_screen_size(mgl_coord_t width,mgl_coord_t height);
#ifdef __cplusplus
}
#endif
#endif //MGL_CORE_H
