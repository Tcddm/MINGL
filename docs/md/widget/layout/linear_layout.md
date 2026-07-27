# 线性布局（MGL_LINEAR_LAYOUT）
线性布局可以以水平或垂直方向排列子组件

## 参数
### 基础参数
<!--@include: @/snippets/widget_base_args.md-->
### 控件独有参数
参数|类型|默认值|说明
-|-|-|-
padding_margin|[mgl_margin_t](/core/types#mgl_margin_t)|(0,0,0,0)|内边距 
direction|[mgl_linear_direction_t](/core/types#mgl_linear_direction_t)|MGL_LINEAR_HORIZONTAL|布局方向
main_align|[mgl_linear_align_t](/core/types#mgl_linear_align_t)|MGL_ALIGN_START|主轴对齐方式
cross_align|[mgl_linear_align_t](/core/types#mgl_linear_align_t)|MGL_ALIGN_START|交叉轴对齐方式
children|const [mgl_widget_blueprint_t](/core/types#mgl_widget_blueprint_t) **|NULL|子控件

### 框架便捷宏参数
参数|类型|默认值|说明
-|-|-|-
painter|[mgl_painter_t](/core/types#mgl_painter_t)|空画笔|背景画笔
<!--@include: @/snippets/widget_round_radius_args.md-->

## API
无

## 动作
<!--@include: @/snippets/event_default_to_action.md-->

## pref_*特殊值说明
### 自然大小（pref_* = 0，默认）
### 沾满剩余空间（pref_* = -1）
#### 行为规则
`-1`在不同布局方向上语义不同：
布局方向|`pref_w = -1`|`pref_h = -1`
-|-|-
**水平布局**|**主轴fill**：排在其他子控件之后，填满剩余宽度|交叉轴 fill：原地不动，高度撑满父容器
**竖布局**|交叉轴fill：原地不动，宽度撑满父容器|**主轴 fill**：排在其他子控件之后，填满剩余高度
::: tip 主轴与交叉轴
主轴是子控件排列的方向（水平布局的主轴是宽度，竖布局的主轴是高度）。交叉轴是垂直方向。

主轴上的`-1`和CSS的`flex-grow: 1`行为一致——该子控件被放到队列最后，拿取剩余的所有空间。交叉轴上的`-1`则是原地膨胀到父容器在垂直方向上的尺寸。
:::
#### 典型用法
```c
//控制栏固定高度，列表沾满剩余空间
MGL_LINEAR_LAYOUT(
    .direction=MGL_LINEAR_VERTICAL,
    .pref_w=g_mgl_screen_width,
    .pref_h=g_mgl_screen_height,
    .children=MGL_CHILDREN(
        MGL_LINEAR_LAYOUT(..., .pref_h = 40),    //控制栏，固定40px
        MGL_LIST(.pref_h = -2, ...)              //列表，填满剩余
    )
)

//侧栏固定宽度，列表沾满剩余
MGL_LINEAR_LAYOUT(
    .direction=MGL_LINEAR_HORIZONTAL,
    .children=MGL_CHILDREN(
        MGL_LINEAR_LAYOUT(..., .pref_w = 80),     //侧栏，固定80px
        MGL_LIST(.pref_w = -2, ...)               //列表，填满剩余
    )
)
```
#### 注意事项
- 根控件（页面最外层）没有父容器提供EXACT约束，-2自动退化为自然大小（和pref_* = 0一致）
- 一个容器中只有一个子控件可以使用主轴的-2。如果有多个，只有第一个生效
- 交叉轴的-2不限数量——多个子控件都可以撑满父容器在垂直方向上的尺寸

## 示例
### 铺满全屏的白色背景
```c
MGL_LINEAR_LAYOUT(
    .pref_w=g_mgl_screen_width,
    .pref_h=g_mgl_screen_height,
    .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_WHITE),
)
```



