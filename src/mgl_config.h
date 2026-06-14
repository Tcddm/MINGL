#ifndef MGL_CONFIG_H
#define MGL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_SDL_CONFIG
#include "../platform/sdl/mgl_config_sdl.h"
#endif

#ifndef MGL_PAGE_STACK_MAX_DEPTH
#define MGL_PAGE_STACK_MAX_DEPTH 8
#endif

#ifndef MGL_MAX_WIDGET_DEPTH
#define MGL_MAX_WIDGET_DEPTH 16
#endif

#ifndef MGL_DIRTY_RECT_MAX_COUNT
#define MGL_DIRTY_RECT_MAX_COUNT 8
#endif

#ifndef MGL_PAGE_POOL_SIZE
#define MGL_PAGE_POOL_SIZE 2048
#endif

#ifndef MGL_SCREEN_WIDTH
#define MGL_SCREEN_WIDTH 480
#endif
#ifndef MGL_SCREEN_HEIGHT
#define MGL_SCREEN_HEIGHT 320
#endif
#ifndef MGL_TOUCH_MAX_FINGERS
#define MGL_TOUCH_MAX_FINGERS 1
#endif
#ifndef MGL_HW_ACCEL_FILL_RECT
#define MGL_HW_ACCEL_FILL_RECT 0
#endif
#ifndef MGL_HW_ACCEL_BIT_BLT
#define MGL_HW_ACCEL_BIT_BLT 0
#endif
#ifndef MGL_FILL_POLYGON_MAX_INTERSECTS //最大交点数
#define MGL_FILL_POLYGON_MAX_INTERSECTS 128
#endif
#ifndef MGL_FPS_LOG
#define MGL_FPS_LOG 1
#endif
#if MGL_FPS_LOG
#define MGL_FPS_REPORT_INTERVAL_MS 2000
#endif
#ifdef __cplusplus
}
#endif
#endif //MGL_CONFIG_H
