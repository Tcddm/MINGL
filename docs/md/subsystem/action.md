# 动作系统
::: tip
动作系统建立在[事件系统](./event)之上，将原始事件翻译为高层动作。
:::
控件的on_event虚函数接收原始事件，但大多数控件只需要处理高层的动作如点击，值变化等。动作系统在事件和控件之间做了一层抽象。将原始事件翻译成动作。
## 设计理念
动作系统不占用控件代码。控件只负责实现on_event处理原始事件，框架在event_bubble末尾自动把事件转换成动作并调用用户注册的回调。控件开发者不需要学两套API。action_handler返回true时，冒泡停止，父控件收不到事件。

## 动作类型
<<< @/../../src/core/mgl_types.h#mgl_action_type_t{c}

## vtable.get_action
控件通过虚表告诉框架什么原始事件→什么动作：
```c
//不设 get_action（默认 NULL）→ 走默认规则

//显式关闭动作
static mgl_action_type_t my_get_action(mgl_widget_t *self,const mgl_event_t *event) {
    return MGL_ACTION_NONE;
}

//自定义动作类型
static mgl_action_type_t slider_get_action(mgl_widget_t *self,const mgl_event_t *event) {
    if(event->type==MGL_EVENT_TOUCH_MOVE){return MGL_ACTION_VALUE_CHANGED;}
    return MGL_ACTION_NONE;
}
```
## 框架自动转换默认规则
框架通过mgl_event_default_get_action函数作为默认的转换规则，现有规则如下：
- TOUCH_UP 转为 MGL_ACTION_CLICK

## 数据在哪里
动作只传类型，不携带数据。因为动作触发时控件的字段已经是新值了。用户在回调里直接读 widget->xxx。

## 回调宏
<<< @/../../src/event/mgl_event.h#MGL_HANDLE_ACTION{c}

<<< @/../../src/event/mgl_event.h#MGL_HANDLE_SINGLE_ACTION{c}

第二个参数是控件类型名（如 mgl_button_t），宏内通过container_of自动恢复真实类型。

## 用法
### 按钮点击
```c
MGL_HANDLE_SINGLE_ACTION(on_click,mgl_button_t,MGL_ACTION_CLICK,{
    MGL_LOG_INFO("Button","clicked");
})

//声明时绑定
MGL_BUTTON(.action_handler=on_click, ...)
```
### 滑块值变化
```c
MGL_HANDLE_ACTION(on_slide,mgl_slider_t,{
    if(action==MGL_ACTION_VALUE_CHANGED){
        printf("value: %d\n",widget->value);
        return true;
    }
    return false;
})

//声明时绑定
MGL_SLIDER(.action_handler=on_slide, ...)
```