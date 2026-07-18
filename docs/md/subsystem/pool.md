# 页面内存池

页面内存池只做一件事：**在页面加载时分配控件内存，在页面卸载时整块释放**。它不维护空闲链表，不处理碎片，不关心分配的对象是什么类型。它就是一个带 4 字节对齐的栈式分配器。

## 它做什么
- 按请求大小顺序分配内存，指针单调递增
- 按释放指针截断栈顶，回退已分配空间
- 一键 reset 清空整块内存

## 它不做什么
- **不支持随机释放** — 只能释放最后一次 `alloc` 的指针（栈式 LIFO）。释放非栈顶指针是未定义行为
- **不在 alloc 时清零内存** — 返回值是上一次 free/reset 后被清零过的，或从未使用过的区域
- **不管分配的内容是什么** — 返回 `void *`，上层自己做类型转换和初始化

## 设计

所有控件内存在一块编译期确定大小的静态数组中：

```c
static uint8_t page_pool[MGL_PAGE_POOL_SIZE]; // 在 Kconfig 中配置
static uint8_t *pool_top = page_pool;          // 栈顶指针
```

之所以用栈式而非堆式，是因为页面切换是栈式的（push/pop），控件的生命周期和页面生命周期严格对应。加载页面时从栈顶分配一批控件，卸载页面时一次性回退到分配前的位置。没有先分配后释放的交叉生命周期，不存在碎片。

### 为什么不用 malloc

| | malloc / free | 页面内存池 |
|--|--------------|-----------|
| 碎片 | 长时间运行产生碎片 | 无碎片（栈式操作） |
| 确定性 | 分配时间不可预测 | allocate 是 O(1) 的指针加法 |
| 内存用量 | 运行时才知道 | 编译期由蓝图树算出，Kconfig 调到刚好够 |
| 依赖 | 需要操作系统级堆管理 | 零依赖，纯静态数组 |
| 调试 | 内存泄漏难追踪 | 页面切换时 reset，不可能泄漏 |

## API

### alloc — 分配

> `void *mgl_page_pool_alloc(uint32_t size)`

从栈顶分配 `size` 字节，自动 4 字节对齐。返回分配区域的起始指针。空间不足时返回 NULL。

<<< @/../../src/pool/mgl_page_pool.c#mgl_page_pool_alloc{c}

```c
mgl_button_t *btn = mgl_page_pool_alloc(sizeof(mgl_button_t));
if (!btn) {
    // 页面池耗尽：增大 MGL_PAGE_POOL_SIZE
}
```

分配的大小按 4 字节向上取整（`ALIGN4` 宏），保证每次分配后栈顶都在 4 字节边界上。这对于 MCU 上需要字对齐访问的架构至关重要。

### free — 释放

> `void mgl_page_pool_free(void *ptr)`

释放从 `ptr` 开始到栈顶的所有内存。操作是截断：`pool_top = ptr`，然后把回退的区域清零。

<<< @/../../src/pool/mgl_page_pool.c#mgl_page_pool_free{c}

::: warning 只能释放栈顶指针
`free` 后 `pool_top` 必须等于 `ptr`。传入非栈顶指针会触发错误日志，池状态不变。这是栈式分配器的核心约束。
:::

释放时用 `memset` 清零回退区域，确保下次 `alloc` 拿到的是干净的（或从没用过的）内存。这意味着控件的位域标志（`dirty`、`visible` 等）在分配后默认为 0。

### reset — 全部清空

> `void mgl_page_pool_reset(void)`

把整个池恢复到初始状态。等价于 `free(page_pool)`，把 `pool_top` 重置为数组起始地址。

<<< @/../../src/pool/mgl_page_pool.c#mgl_page_pool_reset{c}

### get_top — 获取栈顶

> `uint8_t *mgl_page_pool_get_top(void)`

返回当前栈顶指针。主要用途是在页面加载前保存栈顶，加载失败时恢复：

```c
// page/mgl_page.c 中的用法
uint8_t *checkpoint = mgl_page_pool_get_top();
mgl_widget_t *root = mgl_blueprint_collect(blueprint, ...);
if (!root) {
    // 蓝图收集失败，回退到加载前的状态
    mgl_page_pool_free(checkpoint);
}
```

## 配置

::: tip 相关配置
MGL_PAGE_POOL_SIZE
:::

池大小通过 Kconfig 配置，默认 2048 字节。如果页面控件树很大（深层嵌套、大量子控件），日志会输出 `alloc failed` 并提示剩余空间。此时增大 `MGL_PAGE_POOL_SIZE`。

::: tip 如何确定合适的池大小
先设一个保守的大值（如 4096），加载目标页面后观察 `reset` 日志的 `was %u bytes used`，即为该页面实际使用量。然后预留 20%-30% 余量作为最终配置值。编译后余下的 `.bss` 空间就是这个值。
:::

## 在页面加载中的角色

页面池不直接面向用户代码。它被页面管理器在构建控件树时调用：

```
页面 push → 保存栈顶 → alloc 根控件 → alloc 子控件 → ...
          → 如果失败: free(checkpoint) 回退
          → 页面 pop  → free(root) 释放整棵控件树
```

蓝图的 `size` 字段告诉页面池每个控件要分配多大：

<<< @/../../src/widget/mgl_widget_blueprint.h#mgl_widget_blueprint_t{c}

`sizeof(mgl_button_t)` — 每个控件的真实大小在编译期确定，不是在运行时 `sizeof` 的。这也是声明式 DSL 的附带好处：控件的完整内存布局在编译后就已经固定。
