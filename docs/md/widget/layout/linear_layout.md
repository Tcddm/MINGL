# 线性布局（MGL_LINEAR_LAYOUT）
线性布局可以以水平或垂直方向排列子组件

## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-|
padding_margin|[mgl_margin_t](/core/types#mgl_margin_t)|(0,0,0,0)|内边距 
direction|[mgl_linear_direction_t](/core/types#mgl_linear_direction_t)|MGL_LINEAR_HORIZONTAL|布局方向
main_align|[mgl_linear_align_t](/core/types#mgl_linear_align_t)|MGL_ALIGN_START|主轴对齐方式
cross_align|[mgl_linear_align_t](/core/types#mgl_linear_align_t)|MGL_ALIGN_START|交叉轴对齐方式
children|const [mgl_widget_blueprint_t](/core/types#mgl_widget_blueprint_t) **|NULL|子控件

### 框架自带额外参数
参数|类型|默认值|说明
-|-|-|-|
painter|[mgl_painter_t](/core/types#mgl_painter_t)|EMPTY_PAINTER|背景
<!--@include: @/snippets/widget_event_args.md-->
<!--@include: @/snippets/widget_round_radius_args.md-->

## API
无

## 事件
事件类型|触发场景
-|-
MGL_ACTION_CLICK|点击线性布局触发

## 示例
### 铺满全屏的白色背景
```c
MGL_LINEAR_LAYOUT(
    .pref_w=g_mgl_screen_width,
    .pref_h=g_mgl_screen_height,
    .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_WHITE),
)
```



