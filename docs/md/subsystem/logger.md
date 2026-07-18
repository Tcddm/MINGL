# 日志系统
MINGL 的日志系统只做一件事：**在任意位置输出带标签和级别的格式化信息**。它不依赖文件系统、不依赖串口驱动、不依赖操作系统。默认输出到 `printf`，并允许注入自定义输出函数。

## 它做什么
- 提供 DBG / INFO / ERROR 三级日志
- 通过标签区分日志来源（Core / Page / Render / 自定义控件名…）
- 支持 `printf` 格式字符串和可变参数
- 允许替换输出目标（串口、文件、网络、空函数）

## 它不做什么
- **不管理日志存储** — 不写文件、不写 Flash、不维护环形缓冲区。存储策略由用户注入的 `mgl_log_output_t` 决定
- **不按级别过滤** — 所有级别的日志都调用输出函数。如果运行时需要开关，在注入的 `output` 函数中自行判断
- **不自动添加时间戳** — 需要时间戳的话，在调用时手动拼接

## 日志级别
| 级别 | 输出标识 | 使用场景 |
|------|----------|----------|
| `MGL_LOG_LEVEL_DBG` | `DBG` | 布局计算、渲染步骤、事件冒泡轨迹。用于开发调试，量最大 |
| `MGL_LOG_LEVEL_INFO` | `INFO` | 页面加载/卸载、核心初始化完成。正常运行中也有少量输出 |
| `MGL_LOG_LEVEL_ERROR` | `ERROR` | 页面栈溢出、命中测试栈溢出、页面池耗尽。发生了不该发生的事 |

## 日志标签

框架预定义了常用标签，控件可通过 `MGL_LOG_TAG_WIDGET(name)` 宏生成自己的标签：

<<< @/../../src/logger/mgl_log.h#MGL_LOG_TAG{c}

```c
// 控件内部定义自己的标签
#define LOG_TAG MGL_LOG_TAG_WIDGET(linear_layout)

MGL_LOG_DBG(LOG_TAG, "measure start (%p): cw=(%d,%d)", layout, cw, ch);
// 输出: [DBG] [Widget - linear_layout] measure start (0x...) : cw=(100, 200)
```

输出格式固定为 `[级别] [标签] 消息\r\n`：

```
[INFO] [Core] MINGL v1.0 start init (built Jul 18 2026)
[INFO] [Page] start loading main page
[DBG] [Widget - linear_layout] measure result (0x...) : 320x240
[ERROR] [Page Pool] alloc failed: 128 bytes requested but only 48 remain
```

## API

### 输出宏

三个宏的使用方式完全一致，只是级别不同：

```c
MGL_LOG_DBG("Draw", "clip=(%d,%d,%d,%d)", clip.x, clip.y, clip.w, clip.h);
MGL_LOG_INFO("Page", "loading %s page done", name);
MGL_LOG_ERROR("Event", "hit-test stack overflow at depth %d", depth);
```

### 注入自定义输出

<<< @/../../src/logger/mgl_log.h#mgl_log_set_output{c}

默认输出到 `printf`。通过 `mgl_log_set_output` 注入自定义函数后，所有日志调用都会走新的输出：

```c
// 重定向到串口
void uart_output(const char *msg) {
    uart_send_string(msg);
}
mgl_log_set_output(uart_output);

// 运行时关闭所有日志
mgl_log_set_output(NULL); // NULL = 切回 printf
// 或者注入空函数
void silent_output(const char *msg) { (void)msg; }
mgl_log_set_output(silent_output);
```

::: tip
输出函数在 `mgl_log_write` 内部被调用，已经完成了格式化（`[级别] [标签] 消息\r\n`）。注入的函数收到的就是最终要输出的字符串，不需要再做格式化。
:::

### 底层写入函数

<<< @/../../src/logger/mgl_log.h#mgl_log_write{c}

一般不需要直接调用。宏 `MGL_LOG_DBG` 等最终都落到这里。直接调用它的情况通常是实现了自己的日志封装层。

## 在自定义控件中使用

```c
// my_widget.c
#include "logger/mgl_log.h"

#define LOG_TAG MGL_LOG_TAG_WIDGET(my_widget)

static void my_widget_draw(mgl_draw_ctx_t *ctx) {
    my_widget_t *w = container_of(ctx->widget, my_widget_t, base);
    MGL_LOG_DBG(LOG_TAG, "draw (%p): bounds=(%d,%d,%d,%d)",
                w, w->base.bounds.x, w->base.bounds.y,
                w->base.bounds.w, w->base.bounds.h);
    // 实际绘制逻辑...
}
```

标签宏 `MGL_LOG_TAG_WIDGET(my_widget)` 展开为 `"Widget - my_widget"`，和内置控件保持一致的风格。

## 实现细节

日志格式化使用栈上的固定缓冲区（128 字节格式化，256 字节最终输出）。消息超长会被 `vsnprintf` 截断。


::: warning
日志输出的 `\r\n` 是硬编码的。如果你注入的自定义输出函数发往不需要 `\r` 的目标（如 Linux 终端），需要在输出函数中去掉回车符。
:::
