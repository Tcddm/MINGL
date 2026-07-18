# 滚动条（MGL_SCROLLBAR）
::: tip 通常情况下无需手动使用
Scrollbar通常被控件内嵌。用户一般只需要在控件中通过`.scrollbar_args`配置外观即可，无需单独创建。
:::

显示当前位置在滚动内容中的比例，支持点击跳转和拖动。

内容不满屏时自动隐藏。

## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-|
bar_w|[mgl_coord_t](/core/types#mgl_coord_t)|MGL_SCROLLBAR_DEFAULT_BAR_W(4)|滚动条宽度
on_scroll|[mgl_scrollbar_callback_t](/core/types.md#mgl_scrollbar_callback_t)|NULL|滚动回调
callback_data|void *|NULL|回调透传数据

**滚动回调**：当用户拖拽滚动条或滚动位置发生变化时触发

**回调透传数据**：这是一个用户自定义的上下文指针，框架不会修改它，只会在触发on_scroll时原样回传。用于在回调函数中获取外部业务对象、控件实例或状态变量，避免使用全局变量。
### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-|
track|[mgl_painter_t](/core/types#mgl_painter_t)|主题轨道色|轨道背景
thumb|[mgl_painter_t](/core/types#mgl_painter_t)|主题背景色|滑块

## API
```c
/**
 * @brief 设置内容总高度、可见区域高度和当前滚动位置
 *
 * @param sb 滚动条
 * @param content_total 内容总高度
 * @param viewport_h 可见区域高度
 * @param position 当前滚动位置
 */
void mgl_scrollbar_set_content(mgl_scrollbar_t *sb,
                               int32_t content_total,int32_t viewport_h,
                               int32_t position);
```

滚动容器（如 List）调用该函数向滚动条同步数据。滚动条根据内容比例和位置计算滑块高度和偏移。

## 事件
滚动条不通过action_handler回调交互，而是通过on_scroll回调直接将位置返回给滚动容器：
```c
mgl_scrollbar_args_t args={
    .on_scroll=my_callback,
    .callback_data=user_data,
};
```
## 示例
### 在List中使用
List构建时自动创建内嵌滚动条，通过scrollbar_args配置外观：
```c
MGL_LIST(
    .scrollbar_args={
        .bar_w=15,
        .track=MGL_SOLID_PAINTER_TEMP(MGL_COLOR(48,48,48)),
        .thumb=MGL_SOLID_PAINTER_TEMP(MGL_COLOR(200,200,200))
    },
    ...
)
```
