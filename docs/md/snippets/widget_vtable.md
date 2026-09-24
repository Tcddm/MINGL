虚函数|调用时机|作用
-|-|-
draw|[渲染器渲染步骤4](../subsystem/render#4.绘制：并集切块，逐块一次)|绘制自己
measure|布局时|返回自然尺寸
layout|布局时|排列子控件
on_event|事件冒泡|事件处理
get_action|on_event调用后|将原始事件转为动作类型