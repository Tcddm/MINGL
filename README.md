# MINGL

[![Docs](https://img.shields.io/badge/docs-online-brightgreen)](https://tcddm.github.io/MINGL/)

MINGL是一个面向MCU与低性能Linux的嵌入式UI框架。它使用声明式C宏描述界面，渲染器采用多脏区域独立绘制 + 帧级跳过，所有内存和布局在编译期确定，运行时零动态分配。

## 声明式描述界面
以下是一个蓝色背景加居中文本的界面，层次关系一目了然，结构即结果，且IDE可自动补全参数名。

```c
MGL_PAGE_WIDGETS_START(main_page)
MGL_ROOT(
        MGL_LINEAR_LAYOUT(
                .pref_w=g_mgl_screen_width,
                .pref_h=g_mgl_screen_height,
                .main_align=MGL_ALIGN_CENTER,
                .cross_align=MGL_ALIGN_CENTER,
                .round_radius=-1,
                .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_BLUE),
                .children=MGL_CHILDREN(
                        MGL_LABEL(
                            .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_BLACK),
                            .text=MGL_STR_SIZE("Welcome to MINGL!",24)
                        )
                )

        )
)
MGL_PAGE_WIDGETS_END()
```