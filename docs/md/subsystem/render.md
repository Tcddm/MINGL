# 渲染器
::: tip 相关配置
MGL_MAX_WIDGET_DEPTH

MGL_DIRTY_RECT_MAX_COUNT
:::
MINGL的渲染器负责**把脏控件画到屏幕上**，并且**尽量只画必须画的部分**。
## 它做什么
- 在容器内部收集所有脏子控件的边界，每条独立细分成脏矩形（不合并非重叠区域）
- 对每个脏矩形，按控件树的层次顺序绘制背景和内容
- 自动处理一个控件被脏的兄弟/祖先覆盖的情况（波及重绘）

## 它不做什么
- **不管布局** — 布局是`layout()`/`measure()`虚表的职责，渲染器只消费`bounds`
- **不清屏** — 根控件的`draw()`负责画背景，HAL 层负责硬件级别的清屏
- **不管触摸** — 事件系统独立运作，渲染器只负责像素

## 它什么时候运行

每帧刷新时，渲染器都会先检查根控件的`dirty`标志，静止界面完全不消耗渲染资源。只有某个控件被标记脏后，渲染器才会工作。

### 怎么标记需要重绘
- 开发者调用`mgl_widget_set_dirty(w)` — 例如按钮按下时改变颜色
- 开发者调用`mgl_widget_set_dirty_content(w)` — 内容变化，需要重新测量和布局（例如label文本变了可能会改变自身的`bounds`）

脏标记沿父链向上冒泡，最终到达根控件，触发下一帧渲染。

## 渲染流程

<<< @/../../src/page/mgl_render.h#mgl_render_widget{c}

mgl_render_widget是整个渲染器的核心。它以页面根控件为起点，深度优先遍历整棵控件树，对每个控件执行**7**个步骤。

### 遍历架构

渲染器维护三个深度栈（每层一个元素）：
- mgl_rect_t clip_stack[depth]  — 本层有效裁剪区域（父控件bounds ∩ 祖先clip）
- mgl_rect_t clr_stack[depth]   — 本层波及清空区（祖先的绘制覆盖了哪些像素）
- bool clr_valid[depth]   — clr_stack是否有效

遍历采用经典的非递归深度优先算法：
```
w=root,cur_clip=screen,has_clear=false
while(w!=NULL):
    对w执行Step 1-7
    有子控件入栈：w=w->first_child → continue
    无子控件找下一兄弟，无兄弟则回溯到父控件
```
### 1.可见性检查
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step1{c}

跳过不可见控件及其整个子树。不参与脏收集，不参与裁剪计算，不产生任何像素。
在FPS日志中，这些控件的任何变化都不会触发渲染帧。
### 2.布局重排
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step2{c}

触发条件：layout_dirty==1且控件有layout虚表。

#### layout函数的职责：
- 计算每个子控件的新位置/大小
- 对位置发生变化的子控件，先保存 child->prev_bounds=child->bounds（记录旧位置），再赋新值 child->bounds=new_area
- 对移动的子控件设置child->dirty=1（但不冒泡——父容器已经脏了，渲染器会在后续步处理）
如果子控件本身也是容器，递归调用其`layout()`继续排列孙子控件

#### 为什么需要was_layout_dirty快照?

layout执行完会把layout_dirty清零。但Step3需要知道layout是否刚执行过（用于include_self判断）。所以保存快照后再清除。

#### 关键规则
layout_dirty不和dirty一起冒泡。只有以下两种情况会设layout_dirty：
- 父容器的`layout()`函数移动了子容器（级联设置）
- 开发者调用`mgl_widget_set_dirty_content()`并检测到尺寸变化（只设直接父容器）

### 3.收集局部脏矩形
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step3{c}

这是渲染器**最关键**的一步。它遍历所有脏子控件，收集独立的脏矩形。不把两个非重叠的脏子控件合并成一个包围盒——这是MINGL相比传统脏矩形方案的核心优化。

#### 三态决策
|情况|条件|include_self|解释|
|-|-|-|-|
叶子控件|dirty=1|true|自己就是脏源
容器，自己动了|dirty=1, self_changed=1|true|容器整体迁移，需要覆盖轨迹
容器，脏来自子冒泡|dirty=1, self_changed=0, had_child=1|false|脏来自子控件，不自增长脏区域
容器，无脏子控件|dirty=1, had_child=0|true|脏只来自自身（如背景色变）

这是MINGL最重要的设计决策之一：容器因子冒泡变脏时不自增长脏区域，防止一个子控件的局部变动污染整个页面的脏区域。

### 4.与裁剪区求交及绘制背景
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step4{c}

#### 两条路径：
- DIRTY路径：控件自己的脏矩形与父级裁剪区求交
- CLEARED路径：控件不脏，但祖先背景覆盖了自己的区域，必须强制重绘以恢复自身像素

注意：CLEARED路径的draw_area仅和cur_clear求交，不和cur_clip求交。这是设计选择，被波及的控件不应该被父级裁剪区进一步缩小，因为祖先的cur_clear已经代表了确实被画过的区域。

#### 关键细节：
- ctx.clip=draw_area—控件的`draw()`函数收到被限制在draw_area内的裁剪上下文。它不能画到 draw_area之外
- dirty在此处清零，保证下一帧不会重复绘制
- actually_drew标记draw虚表是否被调用。它不关心draw内部是否真的产生了像素（如EMPTY画笔下draw为空操作）。actually_drew=true会触发clear传播，可能导致少量不必要的CLEARED重绘，但不影响正确性

### 5.波及清空区传播
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step5{c}

#### 逻辑：
- 本控件绘制了像素必须告诉子控件祖先在这些区域动了像素
- 如果祖先已有clear区域则合并（不缩小，只扩大）
- 如果本控件没绘制（actually_drew==false）则透传祖先的cur_clear不变

#### 为什么合并
不同祖先在不同层画了像素，每一层的绘制区域是累积的。子控件需要知道所有祖先累计的覆盖区域。

#### 为什么不能缩小
如果祖先画了A区，本控件画了B区（B ⊂ A），给子控件传B会漏掉A-B区域的波及信息，导致那些区域的子控件不重绘。

### 6.递归子控件
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step6{c}

#### 子控件裁剪区：
- 如果本控件绘制了则cur_clip=draw_area（精确到脏区域）
- 如果本控件未绘制则cur_clip=child_area=w.bounds ∩ cur_clip（约束到控件自身范围）

#### 子控件遍历顺序
按first_child → next_sibling链表的自然顺序（即蓝图构建顺序）。每个子控件重新走完Step1-7全流程。
### 7.清理状态
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step7_1{c}

#### 在两种路径执行：
- 叶子控件：无子控件，Step6跳过，执行完Step5后直接到Step7
- 回溯路径：当前控件的所有子控件处理完毕后，回溯到该控件时执行
prev_bounds记录当前帧渲染结束时屏幕上的位置，供下一帧的Step3判断self_changed。

#### 回溯逻辑：
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step7_2{c}

从叶控件开始向上回溯，每层恢复对应的深度栈状态，然后执行Step7清理。最后返回根控件，while(w)退出，整帧渲染结束。

## 绘制原因标识
日志显示|含义|进入条件
-|-|-
DIRTY|控件自身或子控件发生了变化|Step3返回了非零脏矩形
CLEARED|控件不脏，但祖先背景覆盖了本控件区域|has_clear==true且bounds ∩ cur_clear≠∅
不出现|完全跳过|不可见、在裁剪区外、不在波及区内