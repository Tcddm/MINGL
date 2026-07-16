#ifndef MGL_TYPES_H
#define MGL_TYPES_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "mgl_config.h"

#ifdef __cplusplus
extern "C"{
#endif

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

// #region mgl_coord_t
typedef int16_t mgl_coord_t;
// #endregion

extern mgl_coord_t g_mgl_screen_width;
extern mgl_coord_t g_mgl_screen_height;

// #region mgl_rect_t
/**
 * @brief 矩形结构体，用于描述坐标，尺寸和绘制区域
 */
typedef struct{
    mgl_coord_t x; ///<矩形左上角X坐标
    mgl_coord_t y; ///<矩形左上角Y坐标
    mgl_coord_t w; ///<矩形宽度
    mgl_coord_t h; ///<矩形高度
} mgl_rect_t;
// #endregion


typedef struct mgl_widget_t mgl_widget_t;

// #region mgl_draw_ctx_t
/**
 * @brief 绘图上下文结构体
 * @details 保存绘制时的必要信息，所有绘制函数都依赖此结构
 */
typedef struct{
    mgl_rect_t clip; ///<裁剪区域，仅允许在此区域内绘制，防止越界
    mgl_widget_t *widget; ///<当前正在绘制的控件指针
} mgl_draw_ctx_t;
// #endregion

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

// #region mgl_margin_t
/**
 * @brief 边距结构体
 */
typedef struct {
    mgl_coord_t top; ///<顶边距
    mgl_coord_t right; ///<右边距
    mgl_coord_t bottom; ///<底边距
    mgl_coord_t left; ///<左边距
} mgl_margin_t;
// #endregion

// #region mgl_event_type_t
typedef enum{
    MGL_EVENT_TOUCH_DOWN=0,
    MGL_EVENT_TOUCH_MOVE,
    MGL_EVENT_TOUCH_UP
} mgl_event_type_t;
// #endregion

// #region mgl_event_t
typedef struct{
    uint8_t type;
    union{
        struct {
            mgl_coord_t x;
            mgl_coord_t y;
        } touch;
    };
} mgl_event_t;
// #endregion

// #region mgl_action_type_t
typedef enum {
    MGL_ACTION_NONE=0,
    MGL_ACTION_CLICK,
    MGL_ACTION_VALUE_CHANGED
} mgl_action_type_t;
// #endregion


#ifdef __cplusplus
}
#endif
#endif //MGL_TYPES_H
