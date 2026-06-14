#ifndef MGL_TYPES_H
#define MGL_TYPES_H
#include "stdint.h"
#include "stdbool.h"
#include "mgl_config.h"

#ifdef __cplusplus
extern "C"{
#endif

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

typedef int16_t mgl_coord_t;

extern mgl_coord_t g_mgl_screen_width;
extern mgl_coord_t g_mgl_screen_height;

/**
 * @brief 矩形结构体，用于描述坐标，尺寸和绘制区域
 * @param x 矩形左上角X坐标
 * @param y 矩形左上角Y坐标
 * @param w 矩形宽度
 * @param h 矩形高度
 */
typedef struct{
    mgl_coord_t x,y,w,h;
} mgl_rect_t;


typedef struct mgl_widget_t mgl_widget_t;
/**
 * @brief 绘图上下文结构体
 * @details 保存绘制时的必要信息，所有绘制函数都依赖此结构
 * @param clip    裁剪区域，仅允许在此区域内绘制，防止越界
 * @param widget  当前正在绘制的控件指针
 */
typedef struct{
    mgl_rect_t clip;
    mgl_widget_t *widget;
} mgl_draw_ctx_t;

typedef struct{
    mgl_coord_t x;
    mgl_coord_t y;
    bool pressed;
    uint8_t finger_id;
} mgl_touch_point_t;

typedef struct{
    mgl_touch_point_t points[MGL_TOUCH_MAX_FINGERS];
    uint8_t count; // 当前有效点数
} mgl_touch_data_t;

typedef struct{
    uint16_t value;
    uint8_t alpha;
} mgl_color_t;

typedef enum {
    MGL_BITMAP_FORMAT_RGB565=0
} mgl_bitmap_format_t;

typedef struct {
    const uint8_t *data;
    mgl_coord_t w;
    mgl_coord_t h;
    uint8_t format;
} mgl_bitmap_t;

typedef struct {
    mgl_coord_t top,right,bottom,left;
} mgl_margin_t;

typedef enum{
    MGL_EVENT_TOUCH_DOWN,
    MGL_EVENT_TOUCH_MOVE,
    MGL_EVENT_TOUCH_UP
} mgl_event_type_t;

typedef struct{
    uint8_t type;
    union{
        struct {
            mgl_coord_t x;
            mgl_coord_t y;
        } touch;
    };
} mgl_event_t;

typedef enum {
    MGL_ACTION_NONE=0,
    MGL_ACTION_CLICK
} mgl_action_type_t;

typedef struct{
    mgl_action_type_t type;
    mgl_widget_t *widget;
    union {
    };
} mgl_action_event_t;

#ifdef __cplusplus
}
#endif
#endif //MGL_TYPES_H
