# 标签（MGL_LABEL）
标签可以显示文字
## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-|
text|const char *|NULL|显示的文本（注意生命周期）
font|const [mgl_font_t](/core/types.md#mgl_font_t) *|NULL|字体
### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-|
painter|[mgl_painter_t](/core/types#mgl_painter_t)|主题前景色|文本颜色

## API
### 更新文本
```c
/**
 * @brief 更新标签文本
 * 
 * @param label 标签
 * @param text 新文本（注意生命周期）
 */
void mgl_label_set_text(mgl_label_t *label,const char *text);
```

## 动作
<!--@include: @/snippets/event_default_to_action.md-->

## 示例
### 文本
```c
MGL_LABEL(
    .text=MGL_STR("Hello")
)
```
