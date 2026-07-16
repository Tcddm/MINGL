# 事件系统
::: tip
用户使用大多数控件不需要直接处理原始事件，而是通过[动作系统](./action.md)响应高层的点击等操作。
:::
MINGL的事件系统负责**收集输入数据、找到目标控件、沿控件树向上传递事件**。

## 设计思想
MINGL的事件系统采用统一的抽象接口。所有输入事件——无论是触摸还是按键都使用同一个mgl_event_t结构体表示，通过同一个on_event虚函数分发。控件无需关心事件来自何种硬件，只需响应自己感兴趣的事件类型。这种设计让设备驱动代码和控件交互逻辑彻底解耦。

当前版本已完整实现触摸事件，其他输入类型（如按键）在架构上已预留扩展空间。

## 事件类型
<<< @/../../src/core/mgl_types.h#mgl_event_type_t{c}

<<< @/../../src/core/mgl_types.h#mgl_event_t{c}

## 事件生命周期
每一帧事件从HAL到控件经历四个步骤：
### 1.HAL产生原始数据
HAL层通过mgl_touch_data_t结构报告触摸状态。mgl_process_touch_data把原始数据转换成事件。

### 2.命中测试
get_deepest_hit_widget(root,x,y)从根控件出发，遍历控件树，找到坐标下最深层的可见、启用的控件。遍历用显式栈，非递归。

::: warning 栈溢出
如果界面嵌套层数或子控件数过多，命中测试栈可能溢出。溢出时被丢弃的子控件无法响应触摸。
配置MGL_HIT_TEST_MAX_DEPTH可调整栈大小。
:::

### 3. 事件冒泡
event_bubble(target,event)从命中控件开始，沿parent链向上逐级调用on_event虚函数。任何一个控件返回true，冒泡立即停止。

### 4. 动作转换
event_bubble在on_event返回后，做两件事：

1. 确定动作类型
   - 控件设了get_action → 用它返回的动作
   - 没设 → [框架默认转换](./action.md#框架自动转换默认规则)

2. 调用action_handler
   - 动作不是NONE且控件注册了handler → 调handler
   - handler返回true → 事件被消费，冒泡停止
   - handler返回false → 继续向父控件冒泡

## 多点触控
MINGL原生支持多点。HAL层为每个触摸点分配finger_id，框架追踪每个手指的DOWN坐标和当前位置。

- **DOWN**：基于触点坐标做命中测试，事件发给目标控件
- **MOVE/UP**：基于DOWN时的坐标做命中测试（而非当前位置），确保手指滑到其他控件上时事件仍发给最初的目标控件
- **手指数量上限**：由MGL_TOUCH_MAX_FINGERS配置

## 调试日志
日志|级别|含义
-|-|-
bubble ... PASS|DBG|该控件不处理事件，继续冒泡
bubble ... CONSUMED|DBG|该控件消费了事件，冒泡停止
hit-test stack overflow|ERROR|命中测试栈溢出，需增大MGL_HIT_TEST_MAX_DEPTH