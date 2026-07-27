# 开关（MGL_SWITCH）
开关可以响应用户点击并切换开和关两种状态
## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-
value|bool|0|默认状态
track_h|uint8_t|MGL_SWITCH_DEFAULT_TRACK_H(25)|轨道高度
### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-
track|[mgl_painter_t](/core/types#mgl_painter_t)|主题轨道画笔|轨道画笔
thumb|[mgl_painter_t](/core/types#mgl_painter_t)|主题背景画笔|滑块画笔

## API
无

## 动作
动作类型|触发场景
-|-
MGL_ACTION_VALUE_CHANGED|切换状态时触发

## 示例
### 开关
```c
MGL_SWITCH()
```
