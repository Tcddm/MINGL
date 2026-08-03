# 图形缓冲
MINGL有3种帧缓冲模式，分别为无缓冲直接写屏，框架内部单缓冲和框架外部单缓冲。其通过图形化配置中`图形缓冲→帧缓冲模式`选择，默认为无缓冲直接写屏。
## 无缓冲直接写屏
这是最为直接的方式，MINGL的ctx层通过hal层直接操作硬件写屏。
## 框架内部单缓冲
使用此方式MINGL会在内部初始化一个`MGL_SCREEN_WIDTH*MGL_SCREEN_HEIGHT`大小的mgl_color_value_t数组作为缓冲区，框架会提供一部分的hal层实现，开发者需要实现[mgl_hal_flush_display](/hal/index#将所有绘制内容刷新到物理屏幕上)函数将缓冲区的内容刷新到物理屏幕上。

可以通过以下函数获取框架管理的缓冲区：
```c
const mgl_color_value_t *mgl_hal_get_fb(void);
```
## 框架外部单缓冲
此方式行为和框架内部单缓冲类似，只是框架内部不初始化缓冲区，开发者需要自行初始化缓冲区并构造[mgl_fb_t](/core/types#mgl_fb_t)，最后通过以下函数将缓冲区告知框架：
```c
void mgl_hal_set_fb(const mgl_fb_t *fb);
```

## 单缓冲提供的hal层实现
MINGL提供了一部分的hal层的实现，不允许开发者提供的函数用户无法通过配置启用硬件加速并实现，允许开发者提供的则可以启用硬件加速并实现。
### 不允许开发者提供
- mgl_hal_set_pixel
- mgl_hal_fill_rect
- mgl_hal_clear_screen
- mgl_hal_bit_blt
### 允许开发者提供
- mgl_hal_fill_rect_alpha
- mgl_hal_bit_blt_alpha
