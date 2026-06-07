#ifndef MGL_HAL_H
#define MGL_HAL_H

#include "mgl_config.h"
#include "core/mgl_types.h"

#ifdef __cplusplus
extern "C"{
#endif


/*==============================================================================
 * 必须实现的函数（移植者必须提供）
 *============================================================================*/
// #region mgl_hal_set_pixel
/**
 * @brief 在指定坐标绘制一个像素
 * @param x X坐标（从左到右，0 为最左）
 * @param y Y坐标（从上到下，0 为最上）
 * @param color 像素颜色
 */
void mgl_hal_set_pixel(mgl_coord_t x,mgl_coord_t y,mgl_color_t color);
// #endregion

// #region mgl_hal_get_tick_ms
/**
 * @brief 获取系统启动后经过的毫秒数
 * @return 毫秒计数值
 */
uint32_t mgl_hal_get_tick_ms(void);
// #endregion

// #region mgl_hal_get_touch
/**
 * @brief 获取触摸点坐标
 * @param[out] out_x 触摸点的 X 坐标
 * @param[out] out_y 触摸点的 Y 坐标
 * @return true  有触摸/鼠标按下
 *         false 无触摸
 */
bool mgl_hal_get_touch(mgl_touch_data_t *touch);
// #endregion

// #region mgl_hal_flush_display
/**
 * @brief 将绘制内容刷新到物理屏幕
 */
void mgl_hal_flush_display(void);
// #endregion

// #region mgl_hal_clear_screen
/**
 * @brief 清屏
 */
void mgl_hal_clear_screen(void);
// #endregion

/*==============================================================================
 * 可选实现的函数（框架提供默认实现，用户可覆盖）
 *============================================================================*/

// #region mgl_hal_fill_rect
/**
 * @brief 填充矩形区域
 * @param x X坐标
 * @param y Y坐标
 * @param w 矩形宽度（像素）
 * @param h 矩形高度（像素）
 * @param color 填充颜色
 *
 */
void mgl_hal_fill_rect(mgl_coord_t x,mgl_coord_t y,mgl_coord_t w,mgl_coord_t h,mgl_color_t color);
// #endregion

// #region mgl_hal_bit_blt
/**
 * @brief 将位图中的指定矩形区域拷贝到屏幕上的目标坐标
 *
 * 从源位图的 src_rect 区域读取像素，
 * 并将它们绘制到设置的屏幕坐标处。在传输过程中，所有与 transparent_color
 * 颜色相同的像素会被跳过（视为透明）
 *
 * @param x X坐标
 * @param y Y坐标
 * @param src_rect 指向源位图中需要拷贝的矩形区域的指针，其坐标是相对于位图左上角的。
 * @param bmp 指向源位图描述符的指针，包含像素数据、宽高和格式
 * @param transparent_color 需要视为透明的颜色值。任何与这个值相同的像素将不会被绘制。
 */
void mgl_hal_bit_blt(mgl_coord_t x,mgl_coord_t y,const mgl_rect_t *src_rect,const mgl_bitmap_t *bmp,mgl_color_t transparent_color);
// #endregion



#ifdef __cplusplus
}
#endif
#endif /* MGL_HAL_H */