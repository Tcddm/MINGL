# 标签（MGL_LABEL）
标签可以显示文字
## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-
text|const char *|NULL|显示的文本（注意生命周期）
font|const [mgl_font_t](/core/types.md#mgl_font_t) *|NULL|字体
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
