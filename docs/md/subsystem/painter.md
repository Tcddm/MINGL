# 画笔系统
MINGL的画笔系统负责回答**怎么填颜色**。它不是存储颜色的数据结构，而是封装绘制策略的抽象层。画笔是MINGL里唯一知道自己该怎么画的东西。画笔都共享同一个接口，所有形状绘制函数只需调用画笔的方法，完全不关心颜色是怎么来的。

画笔系统的独立，是MINGL实现绘制策略可扩展的关键。添加新画笔类型不需要修改任何形状代码。它让 MINGL在保持极简内核的同时，具备了支撑复杂视觉效果的扩展能力。

:::warning
页面中作为背景的容器目前不支持透明，请使用MGL_THEME_BG()主题画笔或其他画笔
:::

## 画笔系统不是什么
MINGL的画笔系统容易和一些GUI框架的样式系统混淆。它们的根本区别在于职责边界：
- 不是样式系统：样式系统关心的是哪个控件用什么颜色（即颜色数据的组织和管理）。那是主题系统的职责。
- 不是控件外观的单一来源：控件的几何外观由布局系统和绘制函数决定，画笔只负责填充时用什么颜色策略。
- 不绑定单一颜色值：主题画笔在绘制时从当前主题动态解析颜色，支持运行时换肤。

## 画笔定义
### 画笔类型
<<< @/../../src/painter/mgl_painter.h#mgl_painter_type_t{c}

### 画笔虚表
<<< @/../../src/painter/mgl_painter.h#mgl_painter_vtable_t{c}

### 画笔结构体
<<< @/../../src/painter/mgl_painter.h#mgl_painter_t{c}

## 画笔类型
### 纯色画笔
使用纯色画笔一般使用便捷宏来生成复合字面量实现，便捷宏如下：

<<< @/../../src/painter/mgl_solid_painter.h#MGL_SOLID_PAINTER_TEMP{c}

使用：
```c
MGL_LABEL(
    //黑色纯色画笔
    .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_BLACK),
    .text=MGL_STR("Hello")
)
```

### 主题画笔
主题画笔为特殊画笔，他只标记使用的是主题的哪一个画笔，在绘制的时候重定向为正式画笔，便捷宏如下：

<<< @/../../src/painter/mgl_painter.h#theme_define{c}

一般用户不直接使用主题画笔，而是控件作为用户不指定画笔时使用的默认画笔


## 控件如何关联画笔
每个控件在args以及自身结构体中声明MGL_WIDGET_PAINTER_ARGS_FIELD或_NAME变体：
```c
//使用默认名painter
typedef struct {
    ...
    MGL_WIDGET_PAINTER_ARGS_FIELD
} my_widget_args_t;

typedef struct {
    ...
    MGL_WIDGET_PAINTER_FIELD
} my_widget_t;

//自定义名字（foreground）
typedef struct {
    ...
    MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(foreground)
} my_widget_args_t;

typedef struct {
    ...
    MGL_WIDGET_PAINTER_FIELD_NAME(foreground)
} my_widget_t;

```
在init中把画笔复制到控件结构体中，使用MGL_WIDGET_PAINTER_FIELD_HANDLE以及_NAME变体与_DEAFULT变体：
```c
//默认名painter，不指定为空画笔
MGL_WIDGET_PAINTER_FIELD_HANDLE(my_widget,my_widget_args)
//默认名painter，不指定为指定画笔def，使用指针传递
MGL_WIDGET_PAINTER_FIELD_HANDLE_DEFAULT(my_widget,my_widget_args,def)
//自定义名字（foreground），不指定为空画笔
MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(my_widget,my_widget_args,foreground)
//自定义名字（foreground），不指定为指定画笔def，使用指针传递
MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME_DEFAULT(my_widget,my_widget_args,foreground,def)
```

## RGB565颜色定义
<<< @/../../src/painter/mgl_color.h#MGL_COLOR_RGB565{c}