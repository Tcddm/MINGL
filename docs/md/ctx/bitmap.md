# 绘制位图
::: tip 启用硬件加速
如果您的平台支持硬件填充，可以将配置中的**MGL_HW_ACCEL_BIT_BLT**置为**1**并且提供对应实现，可显著提升绘制速度
:::
::: tip 依赖的绘制函数
[mgl_hal_bit_blt](../hal/#2.位图传输)
:::

## 绘制函数

### 绘制位图
<<< @/../../src/draw/mgl_draw_ctx_bitmap.h#mgl_ctx_draw_bitmap{c}

### 绘制位图的部分区域

<<< @/../../src/draw/mgl_draw_ctx_bitmap.h#mgl_ctx_draw_bitmap_part{c}