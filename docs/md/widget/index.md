# 概述
MINGL的控件系统只做一件事：**把声明式宏转换成运行时可渲染的控件树**。开发者用宏描述界面结构，框架负责实例化、布局和绘制。
## 声明式DSL：结构即结果
比较其他嵌入式UI框架和MINGL的差异：
```c
//LVGL——命令式：逐步构造，顺序严格
lv_obj_t *label = lv_label_create(lv_scr_act());
lv_label_set_text(label,"Hello");
lv_obj_set_pos(label,10,20);

//MINGL——声明式：结构即结果，顺序无关
MGL_LABEL(
    .text=MGL_STR("Hello"),
    .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_WHITE)
)
```
命令式需要按顺序执行 create → set_text → set_pos。声明式中，所有属性在宏里一次性指定，A和B的位置在代码中就对应它们在界面上的层级关系。你看代码就知道界面的结构，不需要跟踪运行时状态。

嵌套布局:
```c
MGL_LINEAR_LAYOUT(
            .direction=MGL_LINEAR_VERTICAL,
            .pref_w=g_mgl_screen_width,
            .pref_h=g_mgl_screen_height,
            .main_align=MGL_ALIGN_CENTER,
            .cross_align=MGL_ALIGN_CENTER,
            .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_BLACK),
            .children=MGL_CHILDREN(
                        MGL_LABEL(
                            .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_WHITE),
                            .text=MGL_STR("Welcome!"),
                            .margin=((mgl_margin_t){50,0,50,0}),
                            )
                        )
)            
```
这些宏不是运行时函数调用。它们展开为 C 的复合字面量——静态初始化的数据块。MGL_LINEAR_LAYOUT(...) 展开后是编译期完全确定的 **mgl_widget_blueprint_t** 指针。不会有忘记调用 lv_obj_set_pos的运行时错误，因为位置和大小也在声明宏中指定，由布局系统统一分配。

比如说标签的声明式宏展开如下：
```c
MGL_LABEL(
    .text=MGL_STR("Hello"),
    .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_WHITE)
)

&((mgl_widget_blueprint_t){
        .init=mgl_label_init,
        .size=sizeof(mgl_label_t),
        .args=&(mgl_label_args_t){
            .text=MGL_STR("Hello"),
            .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_WHITE)
        }
    })
```

## 蓝图系统：从声明到实例
声明宏生成的是蓝图（mgl_widget_blueprint_t），不是控件实例。蓝图记录了怎么创建控件的全部信息，但把分配内存和初始化的时机推迟到页面加载的那一刻。

同一个蓝图可以被多个页面引用，每次分配独立的控件实例。

<<< @/../../src/widget/mgl_widget_blueprint.h#mgl_widget_blueprint_t{c}

页面构建时，`mgl_blueprint_collect`函数遍历蓝图树：
- 从页面池分配内存
- 调用控件初始化函数把这块内存初始化为控件实例
- 把实例挂到父控件的first_child链表上
- 返回mgl_widget_t指针，其他和控件有关的系统从此只认这个指针，不关心背后的真实类型

蓝图的三个字段分别回答三个问题：**要分配多大内存？怎么初始化？参数是什么？**

### 页面内存池
:::tip 相关配置
MGL_PAGE_POOL_SIZE
:::
所有控件内存来自一块编译期确定大小的静态数组
```c
static uint8_t page_pool[MGL_PAGE_POOL_SIZE];
```
因为页面是栈式的，所以MINGL的页面内存池也采用栈式内存池。页面加载时从池里顺序分配。页面回收时整块释放并把其所用的内存清零。没有空闲链表，没有碎片。

### 虚表
|虚函数|调用时机|作用|
|-|-|-|
draw|[渲染器渲染步骤4](../subsystem/render#4.与裁剪区求交及绘制背景)|绘制自己
measure|布局时|返回自然尺寸
layout|布局时|排列子控件
on_event|事件冒泡|事件处理|
get_action|on_event调用后|将原始事件转为动作类型|

控件内部通过container_of从基类指针恢复真实类型。编写自定义控件时开发者按需实现这些函数，其余由框架调度。

## 内置控件
|控件类型|控件|用途|
|-|-|-|
layout|[MGL_LINEAR_LAYOUT](./layout/linear_layout)|线性布局
label|MGL_LABEL|显示文本
button|MGL_BUTTON|按钮
slider|MGL_SLIDER|滑块
scrollbar|MGL_SCROLLBAR|滚动条
list|MGL_LIST|列表

这些内置控件和用户自定义控件使用的是同一套虚表机制。它们没有任何特权，只是框架提供给用户的参考实现。