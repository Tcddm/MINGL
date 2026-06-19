# 绘制圆角矩形

## 绘制函数

### 绘制边框圆角矩形
::: tip 依赖的绘制函数
[mgl_ctx_draw_arc](./arc#绘制环形圆弧)

[mgl_ctx_draw_rect](./base#边框矩形)

[mgl_ctx_fill_rect](./base#实心矩形)

:::
<<< @/../../src/draw/mgl_draw_ctx_round_rect.h#mgl_ctx_draw_round_rect{c}

### 绘制实心圆角矩形
::: tip 依赖的绘制函数
[mgl_ctx_fill_arc](./arc#绘制实心扇形圆弧)

[mgl_ctx_fill_rect](./base#实心矩形)
:::
<<< @/../../src/draw/mgl_draw_ctx_round_rect.h#mgl_ctx_fill_round_rect{c}