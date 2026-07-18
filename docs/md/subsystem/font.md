# 字体系统

MINGL 的字体系统只做一件事：**把 Unicode 文本变成屏幕上的像素**。它不负责选择字体、不负责生成字形数据、不关心从哪里加载。它只保存字形位图和描述数据，在绘制时按需查找和渲染。

## 它做什么
- 存储 1bpp 位图字体数据（字形 + 位图）
- 通过 UTF-8 解码 + 二分查找定位字形
- 提供声明宏标记文本，供字体生成工具自动提取字符集

## 它不做什么
- **不生成字形** — 字形由 `tools/font_gen.py` 离线生成到 `generated/font/`
- **不管 TTF** — 运行时没有 TTF 解析器，不依赖 FreeType
- **不支持运行时字号缩放** — 每个字号独立预生成一套字形。MINGL 面向 MCU 场景，不可能在运行时渲染矢量字体

## 数据结构

### 字形

<<< @/../../src/font/mgl_font.h#mgl_glyph_t{c}

| 字段 | 含义 |
|------|------|
| `code_point` | Unicode 码点，二分查找的 key |
| `w` / `h` | 这个字符位图的实际宽高（不是字号。例如 16px 字体中 `i` 的 w 远小于 16） |
| `ofs_x` / `ofs_y` | 绘制时相对基线的偏移。用于对齐不同字符（如 `g` 下沉，`^` 上浮） |
| `adv` | 绘制后光标前进的像素数，决定下一个字符的起始位置 |
| `ofs_bmp` | 在位图数据块中的字节偏移量 |

### 字体

<<< @/../../src/font/mgl_font.h#mgl_font_t{c}

`glyphs` 数组按 `code_point` 升序排列，这是二分查找能 O(log n) 定位字形的前提。字体生成工具保证此顺序。

`bpp` 当前只支持 1（1bpp 位图）。`baseline` 是基线偏移量，配合 `ofs_y` 决定字形在垂直方向上的位置。

## 文本绘制流程

```
UTF-8 字符串 → mgl_utf8_decode 逐字符解码 → 二分查找字形 → mgl_draw_glyph 逐行扫描位图 → set_pixel
```

### UTF-8 解码

绘制文本的入口是 `mgl_ctx_draw_text`。它对字符串逐字符做 UTF-8 解码，支持 1-4 字节的编码（覆盖 BMP 和补充平面）。

::: tip
你的文本可以是中文、英文、符号混排。字体生成工具会扫描所有用到的字符并生成对应的字形。
:::

### 字形查找

每个码点通过二分查找在 `glyphs` 数组中定位。`mgl_glyph_cmp` 只比较 `code_point`，不关心位图数据。

查找失败时（字符不在当前字体中），该字符会画宽和高都为字体大小的方块。

### 字形绘制

对 1bpp 字形，逐行扫描位图字节，找到连续为 1 的 bit 区间后用 `mgl_ctx_fill_rect` 填充。全 0 字节和全 0xFF 字节做了快速路径优化。

所有绘制经过裁剪上下文（`ctx->clip`），不会越界。光标在每次绘制后前进 `adv` 像素：

```c
current_x = (mgl_coord_t)(current_x + glyph->adv);
```

### 文本宽度计算

`mgl_font_get_text_width` 遍历整段文本累加 `adv`，返回像素宽度。用于布局时 `measure()` 计算控件的自然尺寸。
## 声明宏

字体系统对外暴露的核心 API 是四个宏。它们有两个作用：标记文本让字体生成工具提取字符集，以及在控件声明中指定字体。

### MGL_STR / MGL_STR_SIZE

```c
// 16px 默认字号
.text = MGL_STR("Hello")

// 24px 指定字号
.text = MGL_STR_SIZE("Title", 24)
```

`MGL_STR_SIZE("Send", 24)` 展开为 `"Send", .font = &mgl_font_24px`。它同时提供文本内容和字体指针，绑定了"这段文本用多大的字渲染"。宏的值被收集到 `mgl_label_args_t` 的 `text` 和 `font` 字段。

### MGL_FMT / MGL_FMT_SIZE

用于 `sprintf` 格式字符串，如包含数字的动态文本。工具在扫描时自动追加 `0-9` 和 `-` `.` 等常用符号。

```c
// 引用 mgl_font_20px
.text = MGL_FMT_SIZE("%.1f°C", 20)

// 引用默认字号
.text = MGL_FMT("Count: %d")
```

::: warning MGL_FMT / MGL_FMT_SIZE 不等价于 MGL_STR
这两个宏只标记文本和字号，**不生成 `.font` 字段**。带格式字符串的控件需要在 `args` 或 `init` 中自行指定 `font`。当前的内置 label 控件通过 MGL_STR 已经自动提供了 font 指针。
:::

### 字体符号命名规则

`mgl_font_##size##px` — 例如 16px 字号的字体变量名为 `mgl_font_16px`。这些变量由字体生成工具在 `generated/font/mgl_font_gen.h` 中自动声明。启用 `MGL_TOOL_ACCEL_FONT_GEN` 后 `mgl_font.h` 自动引入该生成头。

## 字号管理

### 添加新字号

不需要编辑任何配置文件。只需两步：

1. 在代码中使用 `MGL_STR_SIZE("text", newSize)` 引用新字号
2. 运行 `cmake --build build --target fonts`

工具扫描到新的字号引用后自动生成 `mgl_font_newSizepx` 字体和对应的 `.c` 文件。CMake 的 `file(GLOB)` 自动收集新文件，下次编译生效。

### 字符集

不需要手动维护字符集。工具扫描源码中所有 MGL_STR/MGL_FMT 宏，自动提取字符并去重。

```c
// 写完这行代码后运行 fonts 目标
MGL_LABEL(.text = MGL_STR_SIZE("✓", 16), ...)
// 工具自动生成 16px 的 ✓ 字形，无需任何额外配置
```

::: tip 生成详情
字体生成工具的完整说明见[字体生成工具](../tools/font_gen)。
:::
