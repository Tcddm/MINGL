# 按钮（MGL_BUTTON）
按钮可以响应用户点击，显示文本和图片
## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-|
text|const char *|NULL|显示的文本（注意生命周期）
font|const [mgl_font_t](/core/types.md#mgl_font_t) *|NULL|字体
bmp|const [mgl_bitmap_t](/core/types.md#mgl_bitmap_t) *|NULL|显示的图片
spacing|uint8_t|MGL_BUTTON_DEFAULT_SPACING(4)|图片与文本间距
padding|uint8_t|MGL_BUTTON_DEFAULT_PADDING(8)|水平内边距
### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-|
background|[mgl_painter_t](/core/types#mgl_painter_t)|主题强调色|背景颜色
foreground|[mgl_painter_t](/core/types#mgl_painter_t)|主题前景色|文本颜色
<!--@include: @/snippets/widget_round_radius_args.md-->

## API
无

## 动作
<!--@include: @/snippets/event_default_to_action.md-->

## 示例
### 文本按钮
```c
MGL_BUTTON(
    .text=MGL_STR("Hello")
)
```
