#ifndef MGL_LIST_H
#define MGL_LIST_H

#include "widget/scrollbar/mgl_scrollbar.h"
#include "widget/mgl_widget_blueprint.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef MGL_LIST_POOL_MIN_SIZE
#define MGL_LIST_POOL_MIN_SIZE 3
#endif
#define MGL_LIST_INVALID_INDEX 0xFFFF

// #region mgl_list_adapter_t
typedef struct{
    uint16_t (*get_count)(void *user_data);
    //>0=固定高度，0=自动测量
    mgl_coord_t (*get_height)(void *user_data,uint16_t index);
    //slot_root是item_blueprint实例化出的控件树根
    void (*bind)(void *user_data, mgl_widget_t *slot_root,
                        uint16_t index);
    void *user_data;
} mgl_list_adapter_t;
// #endregion

typedef struct{
    mgl_widget_t *root;
    uint16_t list_index;
} mgl_list_slot_t;

typedef struct {
    const mgl_list_adapter_t    *adapter;
    const mgl_widget_blueprint_t *item_blueprint;

    mgl_scrollbar_args_t scrollbar_args;

    MGL_WIDGET_BASE_ARGS_FIELD
    MGL_WIDGET_PAINTER_ARGS_FIELD
    MGL_WIDGET_ROUND_RADIUS_FIELD
} mgl_list_args_t;

typedef struct {
    mgl_widget_t base;

    mgl_list_adapter_t adapter;
    const mgl_widget_blueprint_t *item_blueprint;
    mgl_scrollbar_t scrollbar;

    //回收池
    mgl_list_slot_t *pool;
    uint8_t pool_size;

    //高度管理(只增不减)
    mgl_coord_t *item_heights;
    int32_t content_height;

    //滚动
    int32_t scroll_y;

    //触摸
    int16_t touch_start_y;
    int32_t scroll_at_start;

    int32_t velocity;
    uint32_t last_move_tick;
    uint8_t animating : 1;

    uint16_t item_count;

    MGL_WIDGET_PAINTER_FIELD
    MGL_WIDGET_ROUND_RADIUS_FIELD
} mgl_list_t;

#define MGL_LIST(...) \
    &((mgl_widget_blueprint_t){ \
        .init=mgl_list_init, \
        .size=sizeof(mgl_list_t), \
        .args=&(mgl_list_args_t){__VA_ARGS__} \
    })

void *mgl_list_init(void *memory, const void *args);

void mgl_list_set_scrollbar(mgl_list_t *list,mgl_scrollbar_t *sb);


/**
 * @brief 滚动到指定行
 *
 * @param list 列表
 * @param index 索引
 */
void mgl_list_scroll_to(mgl_list_t *list,uint16_t index);
/**
 * @brief 获取当前滚动位置
 *
 * @param list 列表
 * @return 当前滚动位置
 */
int32_t mgl_list_get_scroll_position(const mgl_list_t *list);
/**
 * @brief 获取某行的控件树根（可直接修改内容）
 *
 * @param list 列表
 * @param index 索引
 * @return 树根控件指针
 */
mgl_widget_t *mgl_list_get_slot_root(const mgl_list_t *list,
                                     uint16_t index);
/**
 * @brief 某行高度变化时通知列表重建布局
 * @param list 列表
 * @param index 索引
 * @param new_h 新高度
 */
void mgl_list_notify_height_changed(mgl_list_t *list,uint16_t index,
                                    int16_t new_h);

#ifdef __cplusplus
}
#endif
#endif //MGL_LIST_H
