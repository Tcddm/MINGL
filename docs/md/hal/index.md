# 硬件抽象层（HAL）移植指南
硬件抽象层（HAL）是MINGL与具体硬件平台之间的桥梁。通过实现本指南中定义的极简接口，您可以将MINGL移植到任何能点亮一个像素的平台上。
## 核心职责与设计哲学
MINGL的HAL层设计遵循**最小接口原则**，它不关心您用的是什么屏幕，什么触摸芯片，什么操作系统。它只要求您提供最底层的原子操作，例如如何向屏幕写一个像素，如何获取系统时间。所有复杂的图形绘制，裁剪等逻辑，都由MINGL负责。
## 必须实现的函数
### 1.在指定坐标画一个点
::: warning
这是调用频率最高的函数，请尽可能保证其高效
:::
<<< @/../../src/hal/mgl_hal.h#mgl_hal_set_pixel{c}
### 2.返回系统上电后的毫秒时间戳
::: tip
用于动画和时间计算
:::
<<< @/../../src/hal/mgl_hal.h#mgl_hal_get_tick_ms{c}
### 3.将所有绘制内容刷新到物理屏幕上
::: tip
若您使用直接写屏，此函数可以为空；若使用双缓冲/单缓冲，则在此处切换缓冲区或触发DMA传输。 
:::
<<< @/../../src/hal/mgl_hal.h#mgl_hal_flush_display{c}
### 4.清屏
<<< @/../../src/hal/mgl_hal.h#mgl_hal_clear_screen{c}
### 5.触摸
::: tip
如果不支持触摸直接返回false即可
:::
<<< @/../../src/hal/mgl_hal.h#mgl_hal_get_touch{c}
## 可选实现函数
### 1.填充矩形区域
::: tip
如果你有硬件加速实现请将**MGL_HW_ACCEL_FILL_RECT**设置为**1**并且实现该函数
:::
::: tip
默认实现为逐像素填充
:::
::: warning
虽然该函数是可选的，但是作为基础函数的一种还是建议提供硬件加速实现
:::
<<< @/../../src/hal/mgl_hal.h#mgl_hal_fill_rect{c}
### 2.位图传输
::: tip
如果你有硬件加速实现请将**MGL_HW_ACCEL_BIT_BLT**设置为**1**并且实现该函数
:::
::: tip
默认实现为逐像素填充
:::
<<< @/../../src/hal/mgl_hal.h#mgl_hal_bit_blt{c}



