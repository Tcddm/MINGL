# 滑块（MGL_SLIDER）
滑块可以响应用户滑动选择数值
## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-|
value|int16_t|0|当前值
min|int16_t|0|最小值 
max|int16_t|MGL_SLIDER_DEFAULT_MAX(100)|最大值
track_h|uint8_t|MGL_SLIDER_DEFAULT_TRACK_H(4)|轨道高度
thumb_w|uint8_t|MGL_SLIDER_DEFAULT_THUMB_W(24)|滑块宽度 
thumb_h|uint8_t|MGL_SLIDER_DEFAULT_THUMB_W(24)|滑块高度 
### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-|
track|[mgl_painter_t](/core/types#mgl_painter_t)|主题轨道色|轨道颜色
fill|[mgl_painter_t](/core/types#mgl_painter_t)|主题强调色|轨道进度颜色
thumb|[mgl_painter_t](/core/types#mgl_painter_t)|主题背景色|滑块颜色
<!--@include: @/snippets/widget_round_radius_args.md-->

## API
无

## 动作
<!--@include: @/snippets/event_default_to_action.md-->
MGL_ACTION_VALUE_CHANGED|滑块值改变时触发

## 示例
### 滑块
```c
MGL_SLIDER()
```
