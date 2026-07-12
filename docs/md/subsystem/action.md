# 动作系统
::: tip
动作系统建立在[事件系统](./event)之上，将原始事件翻译为高层动作。
:::
控件的on_event虚函数接收原始事件，但大多数控件只需要处理高层的动作如点击，值变化等。动作系统在事件和控件之间做了一层抽象。将原始事件翻译成动作。
## 动作类型
<<< @/../../src/core/mgl_types.h#mgl_action_type_t{c}

<<< @/../../src/core/mgl_types.h#mgl_action_event_t{c}

## 动作回调宏
<<< @/../../src/event/mgl_event.h#MGL_HANDLE_ACTION{c}

<<< @/../../src/event/mgl_event.h#MGL_HANDLE_SINGLE_ACTION{c}

## 动作调度
MGL_WIDGET_ACTION_HANDLE_ACTION宏是负责“翻译”工作的调度器。它被放在控件的on_event虚函数内部，在控件处理完自己的内置逻辑之后调用。它把当前事件转换成对应的动作类型，然后调用控件注册的 action_handler回调。

<<< @/../../src/widget/mgl_widget.h#MGL_WIDGET_ACTION_HANDLE_ACTION{c}

该宏通常放在控件的on_event虚函数末尾。以按钮为例：

<<< @/../../src/widget/button/mgl_button.c#on_event{c}

## 用法
### 按钮点击
```c
//注意按钮要加上.action_handler=on_click 
MGL_HANDLE_SINGLE_ACTION(on_click,mgl_button_t,MGL_ACTION_CLICK,{
    //widget已通过container_of恢复为mgl_button_t*
    MGL_LOG_INFO("Button","on click! text:%s",widget->text);
})
```
