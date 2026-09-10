# 自定义控件指南
在本篇中将会介绍MINGL的控件组织方法以及如何实现自定义控件。
## 控件组织方法
### 宏预定义
使用宏来预定义控件可将减少大量重复代码。
#### 示例
以下宏预定义了一个宽100，高50，圆角半径为20，上下边距都为25，触发动作时调用`handle`函数的文本按钮，并且支持不同的文本id。
```c
#define count_button(btn_text,action_id) \
MGL_BUTTON(
    .pref_w=100, \
    .pref_h=50, \
    .round_radius=20, \
    .text=btn_text, \
    .margin=((mgl_margin_t){0,25,0,25}), \
    .id=action_id, \
    .action_handler=handle \
)
```
比如在计数器界面就可以用下面几条简短代码就能完成3个按钮的定义。
```c
MGL_LINEAR_LAYOUT(
    .pref_w=-1,
    .cross_align=MGL_ALIGN_CENTER,
    .main_align=MGL_ALIGN_CENTER,
    .direction=MGL_LINEAR_HORIZONTAL,
    .children=MGL_CHILDREN(
        count_button(MGL_STR("-"),0),
        count_button(MGL_STR("clear"),1),
        count_button(MGL_STR("+"),2)
    )
)
```
在`handle`函数只需要通过`id`区分按钮即可。
```c
MGL_HANDLE_SINGLE_ACTION(handle,mgl_button_t,MGL_ACTION_CLICK,{
    switch(self->id){
        case 0:
            //-按钮
            break;
        case 1:
            //clear按钮
            break;
        case 2:
            //+按钮
            break;
    }        
})            
```
### 控件组
`MGL_COMPONENT`将一组控件的声明打包为一个命名组件，可在多页面中复用。

和宏预定义不同，控件组不接收参数，它的内部结构在定义时固定。适合导航栏、状态栏、页面模板等不变结构的封装。

```c
//定义
MGL_COMPONENT(kHeader,
    MGL_LINEAR_LAYOUT(
        .direction=MGL_LINEAR_HORIZONTAL,
        .cross_align=MGL_ALIGN_CENTER,
        .painter=MGL_THEME_ACCENT(),
        .pref_w=-1,
        .pref_h=50,
        .children=MGL_CHILDREN(
            MGL_BUTTON(
                .text=MGL_STR("Back"),
                .margin=((mgl_margin_t){0,0,0,10})
            ),
            MGL_LABEL(
                .text=MGL_STR("Title"),
                .margin=((mgl_margin_t){0,0,0,10})
            )
        )
)
);

//使用和普通控件宏完全一样
MGL_LINEAR_LAYOUT(
    .children=MGL_CHILDREN(kHeader,kBody,kFooter)
)
```    
#### 示例
list接收列表项模版就是使用控件组，具体参考[列表示例](./list/list.md#示例)。
## 布局基础
容器控件通过以下四个公共函数来处理子控件的测量和摆放。
函数|调用时机|职责
-|-|-
mgl_layout_linear_measure|容器的measure()|累加子控件尺寸
mgl_layout_linear_layout|容器的layout()|排列子控件
mgl_layout_align_offset|容器的layout()|单子控件的对齐偏移
mgl_layout_align_offset_with_margin|容器的layout()|含margin的对齐偏移

它们是自定义容器控件用来排列子控件的公共工具，LinearLayout的measure/layout内部就调用了它们。自定义容器也可以复用。

## 如何标记控件脏
在MINGL中控件通过3种方式标记脏。
### 控件脏但无bounds变化
控件状态变了但位置和大小没变。最常见——按钮变色、开关翻转。调用`mgl_widget_set_dirty`，该函数会使脏标记沿父链冒泡到根，下一帧仅重绘该控件区域。

<<< @/../../src/widget/mgl_widget.h#mgl_widget_set_dirty{c}

### 控件脏且bounds变化
控件内容变了导致尺寸也跟着变，需要父容器重新布局。调用`mgl_widget_set_dirty_content`，该函数会先自动measure检测尺寸是否真的变了，变了才设父容器`layout_dirty=1`。例如标签文本变长变短、列表行高变化通知。

<<< @/../../src/widget/mgl_widget.h#mgl_widget_set_dirty_content{c}

### 控件脏且要求强制重绘
控件的脏标记来自子冒泡（自身没变），但需要整个bounds被父容器重绘。调用`mgl_widget_set_dirty_full`，该函数会设置`force_redraw=1`让gather收集全bounds而非展开到叶子。极少场景——List slot 回收时需要。

<<< @/../../src/widget/mgl_widget.h#mgl_widget_set_dirty_full{c}

## 自定义控件
前两种方法复用已有控件。当你需要的交互或外观不在内置控件里时，则可以从零实现自定义控件。
### 基类参数介绍
MINGL控件使用了C面向对象思想，所有控件的基类都为`mgl_widget_t`，以下是基类的参数介绍：
参数|类型|默认值|说明
-|-|-|-
vtable|const [mgl_widget_vtable_t](/core/types.md#mgl_widget_vtable_t) *|-|虚表
id|uint16_t|0|控件ID
bounds|[mgl_rect_t](/core/types#mgl_rect_t)|(0,0,0,0)|位置及大小
prev_bounds|[mgl_rect_t](/core/types#mgl_rect_t)|(0,0,0,0)|前位置及大小
margin|[mgl_margin_t](/core/types#mgl_margin_t)|(0,0,0,0)|边距
pref_w|[mgl_coord_t](/core/types#mgl_coord_t)|0|指定宽度（默认为自然大小）
pref_h|[mgl_coord_t](/core/types#mgl_coord_t)|0|指定高度（默认为自然大小）
parent|[mgl_widget_t](/core/types#mgl_widget_t) *|NULL|父控件
first_child|[mgl_widget_t](/core/types#mgl_widget_t) *|NULL|第一个子控件
next_sibling|[mgl_widget_t](/core/types#mgl_widget_t) *|NULL|后一个兄弟
action_handler|[mgl_action_handler_t](/core/types#mgl_action_handler_t)|NULL|动作处理器
dirty|uint8_t:1|0|是否脏
layout_dirty|uint8_t:1|0|是否布局脏
force_redraw|uint8_t:1|0|是否强制重绘
hidden|uint8_t:1|0|是否隐藏
enabled|uint8_t:1|0|是否启用
focused|uint8_t:1|0|是否有焦点
reserved|uint8_t:2|0|保留位
anim_slot|uint8_t|0|动画槽位（暂未使用）
user_data|void *|NULL|用户数据
name|const char *|NULL|控件名字

`name`为可裁剪字段，由`MGL_WIDGET_NAME_FIELD_ENABLE`控制。
#### 初始化函数
基类提供初始化函数以初始化所有基类字段，每个自定义控件都需要在其初始化函数中调用该函数。
```c
void mgl_widget_init(mgl_widget_t *widget,const mgl_widget_vtable_t *vtable,const char *name,void *user_data);
```
### 声明参数
控件中需要2个结构体，一个是控件自身的结构体，另一个是控件参数的结构体，命名通常是加args后缀，比如my_widget_args_t，以下是标签的结构体以及对应的参数结构体：

<<< @/../../src/widget/label/mgl_label.h#mgl_label{c}

可以看出来结构体中并没有每个字段都重复写一遍，因为MINGL提供了许多字段便捷宏，以便开发者减少重复代码，现有的字段便捷宏如下：
#### 参数结构体中声明：
便捷宏|展开
-|-
MGL_WIDGET_BASE_ARGS_FIELD|margin,pref_w,pref_h,id,action_handler
MGL_WIDGET_PAINTER_ARGS_FIELD|const mgl_painter_t *painter;
MGL_WIDGET_PAINTER_ARGS_FIELD_NAME(name)|const mgl_painter_t *name;
MGL_WIDGET_ROUND_RADIUS_FIELD|mgl_coord_t round_radius;

#### 控件结构体中声明：
便捷宏|展开
-|-
MGL_WIDGET_PAINTER_FIELD|mgl_painter_t painter;
MGL_WIDGET_PAINTER_FIELD_NAME(name)|mgl_painter_t name;
MGL_WIDGET_ROUND_RADIUS_FIELD|mgl_coord_t round_radius;

以上的字段便捷宏用来声明字段，因为控件参数结构体在页面实例化完成后就会被丢弃，所以在控件的`init`函数内需要将控件参数结构体的字段赋值到控件结构体，对此MINGL也提供了对应便捷宏简化操作：
- **`MGL_WIDGET_BASE_FIELD_HANDLE(w,args)`**  
复制margin,pref_w,pref_h,id,action_handler
- **`MGL_WIDGET_FIELD_HANDLE(w,args,name)`**  
复制指定字段
- **`MGL_WIDGET_FIELD_HANDLE_DEFAULT(w,args,name,def)`**  
复制指定字段，参数未设时用默认值
- **`MGL_WIDGET_PAINTER_FIELD_HANDLE(w,args)`**  
复制 painter 画笔，未设时为空画笔
- **`MGL_WIDGET_PAINTER_FIELD_HANDLE_DEFAULT(w,args,def)`**  
复制 painter 画笔，未设时用默认值
- **`MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME(w,args,name)`**  
复制指定名字的画笔，未设为空画笔
- **`MGL_WIDGET_PAINTER_FIELD_HANDLE_NAME_DEFAULT(w,args,name,def)`**  
复制指定名字的画笔，未设时用默认值
### 虚表介绍
<!--@include: @/snippets/widget_vtable.md-->
#### draw
`draw`用来绘制控件自身，传入裁剪上下文`ctx`，在该函数中可以使用[裁剪系统](/ctx/index)提供的mgl_ctx_xxx函数结合控件自身的参数来绘制。
```c
static void draw(mgl_draw_ctx_t *ctx){
    mgl_widget_t *self=ctx->widget; //获得基类
    my_widget_t *sw=container_of(self,my_widget_t,base);  //恢复真实类型
    //画背景
    mgl_ctx_fill_rect(ctx, //裁剪上下文
                self->bounds.x,self->bounds.y, //基类参数
                self->bounds.w,self->bounds.h, //基类参数
                &sw->painter //控件独有参数
    );
}
```
::: tip 不需要关心裁剪
mgl_ctx_xxx会通过传入的ctx自动处理裁剪，控件画全bounds即可。
:::

#### measure
`measure`在父布局需要知道控件的尺寸时被调用。父容器传入宽度约束和高度约束，控件根据自身内容和这些约束返回最终尺寸。
##### 三种约束
约束|含义|例子
-|-|-
MGL_MEASURE_EXACT|父要求使用指定值|父容器给fill子控件传EXACT(剩余空间)
MGL_MEASURE_AT_MOST|不能超过指定值|父容器说"最多给你480px宽"
MGL_MEASURE_NONE|无限制，返回自然尺寸|父容器问你"你想要多大"

##### MGL_MEASURE_RESOLVE
该宏处理这三种约束和`pref_w`/`pref_h`的组合。开发者只需要提供自然尺寸，其余由宏处理：
```c
static void measure(mgl_widget_t *self,
                        mgl_measure_constraint_t cw,mgl_measure_constraint_t ch,
                        mgl_coord_t *out_w,mgl_coord_t *out_h){
    my_widget_t *widget=container_of(self,my_widget_t,base);

    mgl_coord_t natural_w=48;   //控件的自然宽度
    mgl_coord_t natural_h=24;   //控件的自然高度

    MGL_MEASURE_RESOLVE(self->pref_w,natural_w,cw,out_w);
    MGL_MEASURE_RESOLVE(self->pref_h,natural_h,ch,out_h);
}
```

#### layout
`layout`在父容器分配了新区域时或`layout_dirty=true`时被调用。容器类控件在这里排列子控件。叶子控件通常不需要实现。

#### on_event
`on_event`接收原始触摸事件。返回true表示消费了事件（停止冒泡），false表示不关心。框架在 on_event返回后自动调`get_action`转换动作。
```c
static bool on_event(mgl_widget_t *self,const mgl_event_t *event){
    my_widget_t *widget=container_of(self,my_widget_t,base);

    if(event->type==MGL_EVENT_TOUCH_UP){
        //抬起时的操作
        return true;
    }
    return false;
}
```
#### get_action
get_action负责将原始事件转换为业务动作。不设时框架会使用[默认转换](/subsystem/action.md#框架自动转换默认规则)。想跳过默认行为可直接返回MGL_ACTION_NONE。
```c
static mgl_action_type_t get_action(mgl_widget_t *self,const mgl_event_t *event){
    return MGL_ACTION_NONE;
}
```
### 示例：开关控件
以下以开关控件（Switch）为例，展示从零实现一个控件的完整流程。
#### 声明控件结构体及参数结构体
首先我们规划开关控件需要什么参数：
参数|原因
-|-
基础参数|必须有，否则用户体验不佳
默认状态|用户需要决定默认开还是关
轨道高度|绘制使用
轨道画笔|绘制使用
滑块画笔|绘制使用

所以最终的控件结构体及参数结构体如下：

<<< @/../../src/widget/switch/mgl_switch.h#struct{c}
#### 实现init函数
接下来开始实现控件的`init`函数，函数签名如下：
```c
void *mgl_switch_init(void *memory,const void *args); 
```
`init`函数在[蓝图系统](/widget/index.md#蓝图系统从声明到实例)构建控件时调用,用于将参数结构体的字段赋值到控件结构体，同时也可以进行控件内部的初始化。因为在`init`函数中需要初始化基类，所以需要先创建好虚表的结构体：
```c
mgl_widget_vtable_t vtable={

};
```

由于开关控件不需要内部初始化，所以其init函数只需负责将参数结构体的字段赋值到控件结构体，具体的实现如下：

<<< @/../../src/widget/switch/mgl_switch.c#init{c}

:::tip 如何提供默认值
如果控件的参数需要默认值，但是又不想写死在代码中的话，可以在控件的h文件添加宏来实现
```c
#ifndef MGL_SWITCH_DEFAULT_TRACK_H
#define MGL_SWITCH_DEFAULT_TRACK_H 25
#endif
```
:::

#### 实现声明宏
用于简化创建mgl_widget_blueprint_t对象并拿到它指针。
```c
#define MGL_SWITCH(...) \
    &((mgl_widget_blueprint_t){ \
        .init=mgl_switch_init, \
        .size=sizeof(mgl_switch_t), \
        .args=&(mgl_switch_args_t ){ __VA_ARGS__ } \
    })
```
#### 实现虚表
##### draw
![draw简化图](/img/switch/switch_draw.png "draw简化图")
可以看出开关控件由一个轨道加一个滑块组成，轨道是一种特殊的圆角矩形，滑块是一个圆，具体的实现如下：

<<< @/../../src/widget/switch/mgl_switch.c#draw{c}

##### measure
开关控件的自然尺寸由轨道高度决定：宽=高×2。

<<< @/../../src/widget/switch/mgl_switch.c#measure{c}

##### layout

开关控件不需要layout

##### on_event
TOUCH_UP时翻转value并标脏，触发下一帧重绘。返回`true`停止冒泡。

<<< @/../../src/widget/switch/mgl_switch.c#on_event{c}

##### get_action
TOUCH_UP时返回`VALUE_CHANGED`。

<<< @/../../src/widget/switch/mgl_switch.c#get_action{c}

##### 最终虚表

<<< @/../../src/widget/switch/mgl_switch.c#vtable{c}

#### 测试控件
在主页面中声明动作处理：
```c
MGL_HANDLE_SINGLE_ACTION(switch_test,mgl_switch_t,MGL_ACTION_VALUE_CHANGED,{
    MGL_LOG_INFO("main","value:%d",widget->value);
});
```
之后在主页面中声明：
```c
MGL_SWITCH(.action_handler=switch_test)
```
运行可看到开关控件，通过点击可以切换状态：
![开关关闭](/img/switch/switch_off.png "开关关闭")
![开关打开](/img/switch/switch_on.png "开关打开")
点击的同时控制台会输出开关的状态：
![控制台输出](/img/switch/action_test.png "控制台输出")