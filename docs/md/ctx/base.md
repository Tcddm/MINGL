# 基础绘制
基础绘制包含了点，线，面的裁剪绘制函数，后续高级图形通过调用基础绘制函数来实现裁剪和硬件加速。
## 初始化上下文
<<< @/../../src/draw/mgl_draw_ctx.h#mgl_ctx_init{c}

## 全屏绘图上下文
该模块提供了全局变量来表示全屏的绘图上下文，在初始化和尺寸变更时会自动更新。

<<< @/../../src/draw/mgl_draw_ctx.h#g_mgl_full_screen_ctx{c}

## 画点
::: warning
此函数每次调用都会进行裁剪判断，**高频调用时性能开销不可忽略**
:::

<<< @/../../src/draw/mgl_draw_ctx.h#mgl_ctx_draw_pixel{c}

## 画线
::: tip 依赖的绘制函数
[mgl_ctx_fill_rect](./base#实心矩形)
:::
<<< @/../../src/draw/mgl_draw_ctx.h#mgl_ctx_draw_line{c}

## 画矩形
### 边框矩形
::: tip 依赖的绘制函数
[mgl_ctx_fill_rect](./base#实心矩形)
:::
<<< @/../../src/draw/mgl_draw_ctx.h#mgl_ctx_draw_rect{c}
### 实心矩形
::: tip 启用硬件加速
如果您的平台支持硬件填充，可以将配置中的**MGL_HW_ACCEL_FILL_RECT**置为**1**并且提供对应实现，可显著提升绘制速度，由于许多高级图形绘制都依赖于该函数，所以硬件支持最好提供硬件加速实现
:::
<<< @/../../src/draw/mgl_draw_ctx.h#mgl_ctx_fill_rect{c}