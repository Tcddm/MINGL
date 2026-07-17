# 主题系统
::: tip
主题系统建立在[画笔系统](./painter)之上，提供默认的画笔和参数。
:::
主题系统通过一个全局指针提供默认画笔和参数。切换主题时改一次指针、触发全屏重绘，所有控件自动刷新。

## 设计
只有一个全局变量g_mgl_theme指向当前主题。用MGL_THEME_FG()等宏存储主题槽位索引，draw时由ctx函数实时解析。

## 主题结构体
<<< @/../../src/theme/mgl_theme.h#mgl_theme_t{c}

## 内置主题
主题|变量|说明
-|-|-
亮色|g_mgl_theme_light|默认主题，浅色背景
暗色|g_mgl_theme_dark|深色背景，浅色文字

## 切换主题
```c
mgl_theme_set(&g_mgl_theme_dark);
```
## 主题槽位枚举
<<< @/../../src/painter/mgl_painter.h#mgl_theme_slot_t{c}

## 便捷宏
<<< @/../../src/painter/mgl_painter.h#theme_define{c}