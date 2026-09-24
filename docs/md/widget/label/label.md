# 标签（MGL_LABEL）
标签可以显示文字
## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-
text|const char *|NULL|显示的文本（注意生命周期；支持 `\n` 换行）
font|const [mgl_font_t](/core/types.md#mgl_font_t) *|NULL|字体
line_spacing|mgl_coord_t|0|行间距（额外像素）
text_align_h|mgl_text_align_t|MGL_TEXT_ALIGN_START|水平对齐
text_align_v|mgl_text_align_t|MGL_TEXT_ALIGN_START|垂直对齐
### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-
painter|[mgl_painter_t](/core/types#mgl_painter_t)|主题前景画笔|文本画笔

## API
### 更新文本
`label`为标签控件指针

`text`为标签存文本的字段

`"Hello"`为新文本，需要注意文本的生命周期
```c
mgl_widget_set_text(label,text,"Hello");
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

### 多行与对齐
```c
MGL_LABEL(
    .text=MGL_STR("Line1\nLine2"),
    .text_align_h=MGL_TEXT_ALIGN_CENTER,
    .text_align_v=MGL_TEXT_ALIGN_CENTER,
    .line_spacing=2
)
```
