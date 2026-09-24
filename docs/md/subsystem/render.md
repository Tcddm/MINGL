# 渲染器
::: tip 相关配置
MGL_MAX_WIDGET_DEPTH

MGL_DIRTY_RECT_MAX_COUNT

MGL_LOG_ENABLE_RENDER / MGL_LOG_ENABLE_RENDER_DETAIL / MGL_LOG_ENABLE_LAYOUT

MGL_FPS_LOG
:::
MINGL的渲染器负责**把脏控件画到屏幕上**，并且**尽量只画必须画的部分**。

## 它做什么
- 在容器内部收集所有脏子控件的边界，每条独立细分成脏矩形（不合并非重叠区域）
- 对每个控件，把它「自己变脏的区域」和「被祖先覆盖、需要恢复的区域」求**并集**，切成两两不相交的块，每块只调用一次`draw()`
- 自动处理一个控件被脏的兄弟/祖先覆盖的情况（波及重绘），并保证**同一像素每帧最多被合成一次**——半透明背景不会因为重复合成而变深

## 它不做什么
- **不管布局** — 布局是`layout()`/`measure()`虚表的职责，渲染器只消费`bounds`
- **不清屏** — 根控件的`draw()`负责画背景，HAL 层负责硬件级别的清屏
- **不管触摸** — 事件系统独立运作，渲染器只负责像素

## 它什么时候运行

每帧刷新时，渲染器都会先检查根控件的`dirty`标志，静止界面完全不消耗渲染资源。只有某个控件被标记脏后，渲染器才会工作。

### 怎么标记需要重绘
- 开发者调用`mgl_widget_set_dirty(w)` — 例如按钮按下时改变颜色
- 开发者调用`mgl_widget_set_dirty_content(w)` — 内容变化，需要重新测量和布局（例如label文本变了可能会改变自身的`bounds`）
- 开发者调用`mgl_widget_set_dirty_full(w)` — 要求整个`bounds`强制重绘且重算布局（例如切换到新主题、列表槽位回收）。它设置的`force_redraw`是**一次性**的：渲染器在处理完该控件后会把它清零，不会跨帧残留

脏标记沿父链向上冒泡，最终到达根控件，触发下一帧渲染。

## 渲染流程

<<< @/../../src/page/mgl_render.h#mgl_render_widget{c}

mgl_render_widget是整个渲染器的核心。它以页面根控件为起点，深度优先遍历整棵控件树，对每个控件执行**7**个步骤。

### 遍历架构

渲染器维护三个深度栈（每层一个元素）：
- `clip_stack[depth]` — 本层有效裁剪区域（父控件 bounds ∩ 祖先 clip）
- `cur_clear[MGL_DIRTY_RECT_MAX_COUNT]` — 本层波及清空区（祖先的绘制覆盖了哪些像素）
- `clr_stack[depth][MGL_DIRTY_RECT_MAX_COUNT]` / `clr_count_stack[depth]` — 保存/恢复`cur_clear`的深度栈

遍历采用经典的非递归深度优先算法：
```
w=root, cur_clip=screen, cur_clear_count=0
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

触发条件：`layout_dirty==1`且控件有`layout()`虚表。

#### layout函数的职责：
- 计算每个子控件的新位置/大小
- 对位置发生变化的子控件，先保存`child->prev_bounds=child->bounds`（记录旧位置），再赋新值`child->bounds=new_area`
- 对移动的子控件设置`child->dirty=1`（但不冒泡——父容器已经脏了，渲染器会在后续步处理）

如果子控件本身也是容器，递归调用其`layout()`继续排列孙子控件。

#### 关键规则
`layout_dirty`不和`dirty`一起冒泡。只有以下两种情况会设`layout_dirty`：
- 父容器的`layout()`函数移动了子容器（级联设置）
- 开发者调用`mgl_widget_set_dirty_content()`/`mgl_widget_set_dirty_full()`，前者在检测到尺寸变化时设置，后者总是设置

### 3.收集局部脏矩形
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step3{c}

它遍历所有脏子控件，收集独立的脏矩形。**不把两个非重叠的脏子控件合并成一个包围盒**——这是MINGL相比传统脏矩形方案的核心优化。

#### 三态决策

|情况|条件|include_self|解释|
|-|-|-|-|
|叶子控件|dirty=1|true|自己就是脏源|
|容器，自己动了|dirty=1, self_changed=1|true|容器整体迁移，需要覆盖轨迹|
|容器，脏来自子冒泡|dirty=1, self_changed=0, had_child=1, force_redraw=0|false|脏来自子控件，不自增长脏区域|
|容器，无脏子控件|dirty=1, had_child=0|true|脏只来自自身（如背景色变）|

容器因子冒泡变脏时不自增长脏区域，防止一个子控件的局部变动污染整个页面的脏区域。

> 收集结果受`MGL_DIRTY_RECT_MAX_COUNT`限制：当条目数达到上限时，后续矩形会被并入最后一条（形成包围盒），避免数组无限增长。

### 4.绘制：并集切块，逐块一次
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step4{c}

Step4把本控件本帧需要重画的像素集合定义为一个**并集**：

```
repaint = (dirty_rects[i] ∩ cur_clip)  ∪  (bounds ∩ cur_clear[i])
```

其中：
- `dirty_rects`是Step3收集到的自身/子控件的脏区
- `cur_clear`是祖先绘制过、覆盖到本控件的区域（背板被覆盖，需要强制重画以恢复自身像素）

然后把这组可能互相重叠的矩形，切成**两两不相交**的块，对每块调用一次`draw()`。

#### 为什么必须是并集、且不相交
- 对不透明画笔，重复绘制同一个像素是幂等的，无所谓；
- 对**半透明画笔**，同一像素被同一个控件合成两次会叠加两次，颜色变深。所以渲染器必须保证「每个像素每帧最多合成一次」，也就是：重画区域取并集、切成不相交块、每块只画一次。

#### 快速路径（按顺序）
`render_paint_union()`会尽量走廉价分支，只有真正部分交叉才做完整拆块：
1. 只有 1 块 → 直接画；
2. `render_merge_bands()`：把**同`x`、同`w`、纵向相邻/重叠**的矩形精确并成一条带（典型场景：滚动列表里相邻 item 的`prev∪bounds`叠段），反复扫描到稳定；
3. 剩 2 块 → 包含则画大的；不相交则各画一次；否则拆块；
4. `render_compact_rects()`：删除被其它矩形**完全包含**的冗余矩形；
5. 剩余矩形两两不相交 → 各画一次；
6. 否则 `render_paint_disjoint()`：坐标压缩 + 按 y 条带扫描，输出两两不相交的矩形。

#### 关键细节
- 每个输出块通过`render_paint_rect()`调用`draw()`，`ctx.clip`即该块——`draw()`不能画到块外；
- 每个块同时被追加进`clear_rects`（传给子控件）和`flush_rects`（交给 HAL），追加时用`render_add_rect_dedup()`保持“互不包含”；
- 处理完本控件后`w->dirty`清零。

### 5.波及清空区传播
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step5{c}

本控件画过的区域，意味着子控件的像素被覆盖，必须通知子控件重画。

#### 逻辑：
- 把本层`clear_rects`逐条与`w->bounds`求交，得到给子控件的`next_clear`；
- 追加时用`render_add_rect_dedup()`：新块被已有块包含则跳过，包含已有块则替换，从而保持`next_clear`**互不包含**、尽量紧凑，避免冗余矩形一路传下去。

#### 为什么不能缩小
如果祖先画了A区，本控件画了B区（B ⊂ A），给子控件传B会漏掉A-B区域的波及信息，导致那些区域的子控件不重绘。去重只删除「并集不变」的冗余块，不会缩小覆盖范围。

### 6.递归子控件
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step6{c}

#### 子控件裁剪区：
- `cur_clip = w->bounds ∩ cur_clip`（约束到控件自身范围），子控件不能画到父控件之外
- 子控件继承的`cur_clear = next_clear`

#### 子控件遍历顺序
按`first_child → next_sibling`链表的自然顺序（即蓝图构建顺序）。每个子控件重新走完Step1-7全流程。
### 7.清理状态
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step7_1{c}

#### 执行时机：
- 叶子控件：无子控件，Step6跳过，执行完Step5后直接到Step7
- 回溯路径：当前控件的所有子控件处理完毕后，回溯到该控件时执行

清理`dirty`与`force_redraw`（保证强制重绘是一次性的），并更新`prev_bounds`记录当前帧渲染结束时屏幕上的位置，供下一帧的Step3判断`self_changed`。

#### 回溯逻辑：
<<< @/../../src/page/mgl_render.c#mgl_render_widget_step7_2{c}

从叶控件开始向上回溯，每层恢复对应的深度栈状态，然后执行Step7清理。最后返回根控件，while(w)退出，整帧渲染结束。

## 日志

渲染相关日志由以下开关控制（都还需总开关`MGL_LOG_ENABLE_DBG=1`）：

| 配置 | 内容 |
|-|-|
|`MGL_LOG_ENABLE_RENDER`|每帧粗粒度日志：`render start` / `render done (%ums)(flush: %ums)`|
|`MGL_LOG_ENABLE_RENDER_DETAIL`|逐控件/逐块细节：`draw widget(%p): clip=(...) PAINT`|
|`MGL_LOG_ENABLE_LAYOUT`|布局日志：`layout widget(%p): bounds=(...)`|
|`MGL_LOG_ENABLE_EVENT`|事件冒泡日志（事件子系统）|
|`MGL_FPS_LOG`|每`MGL_FPS_REPORT_INTERVAL_MS`毫秒输出一次帧率与耗时统计|

## 绘制原因标识
`MGL_LOG_ENABLE_DBG`+`MGL_LOG_ENABLE_RENDER_DETAIL`开启时，每次实际绘制会打印一行：

| 日志显示|含义|进入条件|
|-|-|-|
|PAINT|该控件在某个不相交块上被绘制|该块属于`repaint`并集|
|不出现|完全跳过|不可见、在裁剪区外、或`repaint`并集为空|
