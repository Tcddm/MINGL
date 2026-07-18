# 核心工具 API
核心工具包含两个模块：**矩形操作**负责屏幕坐标的几何计算，**数学工具**提供查表三角函数、排序和二分查找。它们是框架内部使用最频繁的基础设施，不直接依赖任何其他子系统。

## 矩形操作

矩形操作只做一件事：**对 `mgl_rect_t` 做几何运算**。所有布局、裁剪、渲染的坐标计算最终都落到这两个函数上。

### 它做什么
- 判断两个矩形是否有重叠区域，并计算交集
- 计算两个矩形的包围盒（并集）

### 它不做什么
- **不管像素** — 返回的是抽象矩形，不涉及任何绘制
- **不管控件** — 矩形就是矩形，不关心它属于哪个控件
- **不修改输入** — 所有运算结果通过 `out` 参数返回，输入不被修改

### 数据结构

矩形由左上角坐标和宽高定义：

<<< @/../../src/core/mgl_types.h#mgl_rect_t{c}

坐标类型 `mgl_coord_t` 为 `int16_t`，可表示 -32768 到 32767 的范围，足够覆盖常见嵌入式屏幕。

### 相交

<<< @/../../src/core/mgl_rect.h#mgl_rect_intersect{c}

两个矩形相交时 `out` 写入重叠区域并返回 `true`，不相交时返回 `false`，`out` 内容未定义。

```c
mgl_rect_t a = {0, 0, 100, 100};
mgl_rect_t b = {50, 50, 100, 100};
mgl_rect_t result;
if (mgl_rect_intersect(&a, &b, &result)) {
    // result = {50, 50, 50, 50}
}
```

**核心用法是裁剪**：在渲染流程中，每个脏矩形必须先和父级裁剪区求交，确保绘制不越界。

```c
// draw/mgl_draw_ctx.c 中的典型用法
mgl_rect_t draw_area;
if (mgl_rect_intersect(&rect, &ctx->clip, &draw_area)) {
    // 只画 draw_area 范围内的像素
}
```

### 并集

<<< @/../../src/core/mgl_rect.h#mgl_rect_union{c}

并集定义为同时包含两个矩形的最小矩形。在渲染器的脏区域收集中用于合并子控件的脏范围：

```c
// page/mgl_render.c 中的典型用法 — 收集脏子控件的包围矩形
mgl_rect_union(&c->prev_bounds, &c->bounds, &rects[count]);
// 既覆盖旧位置，也覆盖新位置，保证移动动画不残影
```

::: tip 为什么同时取 prev_bounds 和 bounds
控件移动时，旧位置的像素需要被父控件背景覆盖，新位置需要画新内容。取两者的并集作为脏区域，确保两处都被处理。
:::

## 数学工具

数学工具提供 MINGL 内部所需的基础算法。没有浮点数，所有运算基于定点整数。

### 三角函数

<<< @/../../src/core/mgl_math.h#mgl_sin_i16{c}

`mgl_sin_i16` 用 91 点的查表实现正弦，输入为角度（0-359），返回值为 `sin(θ) × 32767` 的定点数。余弦通过相位偏移 90° 实现：

```c
static inline int16_t mgl_cos_i16(int16_t angle) {
    return mgl_sin_i16((int16_t)(angle + 90));
}
```

主要用在圆弧绘制中计算扫描线上每个像素的坐标：

```c
// draw/mgl_draw_ctx_arc.c 中的典型用法
*x = (mgl_coord_t)(cx + ((int32_t)radius * mgl_cos_i16((int16_t)angle) >> 15));
*y = (mgl_coord_t)(cy + ((int32_t)radius * mgl_sin_i16((int16_t)angle) >> 15));
```

::: tip 精度说明
`>> 15` 等价于除以 32768。90° 的理论值是 32768，但 `int16_t` 上限是 32767。表格用 32767 代替，精度损失远低于一个像素，对屏幕绘制无影响。
:::

查表大小仅 91 个 `uint16_t`（182 字节），远小于运行时计算 `sin()` 的代码膨胀，也不需要链接 `libm`。

### 排序

<<< @/../../src/core/mgl_math.h#mgl_sort{c}

冒泡排序，对 `int16_t` 数组原地排序。用于填充多边形的扫描线算法中对交点 X 坐标排序：

```c
// draw/mgl_draw_ctx_fill_polygon.c 中的典型用法
mgl_sort(xi, n); // 每行交点按 X 坐标排序后两两配对填充
```

::: warning
冒泡排序的时间复杂度是 O(n²)。填充多边形的每行交点数通常不超过 `MGL_FILL_POLYGON_MAX_INTERSECTS`（默认 128），且大部分情况下实际交点数远小于此值。如果多边形极其复杂，可增大该配置。
:::

### 二分查找

<<< @/../../src/core/mgl_math.h#binary_search{c}

标准二分查找，返回匹配元素的指针，未找到返回 NULL。用于在字体字形表中按 Unicode 码点查找字形：

```c
// draw/mgl_draw_ctx_text.c 中的典型用法
mgl_glyph_t target = { .code_point = ch };
mgl_glyph_t *glyph = binary_search(
    font->glyphs, font->glyph_count, sizeof(mgl_glyph_t), &target, glyph_cmp);
```

`glyph_cmp` 是用户提供的比较函数，只需比较 `code_point` 字段。字形表必须按码点升序排列——字体生成工具保证这一点。

## 静态工具宏

### MGL_ABS_I16

<<< @/../../src/core/mgl_math.h#MGL_ABS_I16{c}

求 `int16_t` 绝对值。中间提升到 `int32_t` 避免 `-32768` 取负时的溢出：

```c
int16_t result = MGL_ABS_I16(-100); // 100
```

### container_of

<<< @/../../src/core/mgl_types.h#container_of{c}

从成员指针恢复包含它的结构体指针。MINGL 的虚表机制重度依赖此宏：

```c
// 已知 base 是 mgl_widget_t *，恢复真实类型
mgl_button_t *btn = container_of(base, mgl_button_t, base);
```

因为控件通过基类指针（`mgl_widget_t *`）传递，`draw()` / `on_event()` 等虚函数内部第一步就是用 `container_of` 恢复真实类型，然后读取控件自身的字段。
