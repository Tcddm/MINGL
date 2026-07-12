# 页面定义指南
页面是MINGL中承载UI的基本单元。每个页面包含一棵控件树，由MINGL的页面管理器统一调度和切换。本节介绍如何用MINGL的声明式宏定义页面、注册页面到路由表，以及推荐的文件组织方式。
## 定义页面

### 前置准备
::: tip
建议控件的h文件在c文件中导入，避免污染
:::
将页面对应的c文件和h文件创建完成，在h文件中导入page/mgl_page_manager.h
### 页面的c文件布局
页面的c文件通常分为两部分：
- **上半部分**：页面逻辑，包括事件处理函数、数据绑定逻辑等
- **下半部分**：页面的控件声明区域，使用MINGL的声明式宏描述界面结构
### 声明页面的控件
使用MGL_PAGE_WIDGETS_START和MGL_PAGE_WIDGETS_END宏来包裹页面的控件声明区域，其中`my_page`为页面名
```c
MGL_PAGE_WIDGETS_START(my_page)
/*
控件声明
*/
MGL_PAGE_WIDGETS_END()
```
根控件的声明用的是MGL_ROOT宏，根控件在该宏内定义
::: warning
MINGL只支持一个根组件。如果页面需要多个控件，必须通过布局类控件来组织子控件。
:::
如铺满全屏的白色背景页面的定义如下：
```c
MGL_PAGE_WIDGETS_START(my_page)
MGL_ROOT(
    MGL_LINEAR_LAYOUT(
        .pref_w=g_mgl_screen_width,
        .pref_h=g_mgl_screen_height,
        .painter=MGL_SOLID_PAINTER_TEMP(MGL_COLOR_WHITE)
    )
)
MGL_PAGE_WIDGETS_END()
```
### 声明页面构建函数
使用MGL_EXPORT_PAGE宏在页面对应的h文件中声明页面构建函数，以便其他文件（如注册表）能够引用该页面：
```c
MGL_EXPORT_PAGE(my_page)
```
## 注册页面
使用MGL_PAGE_REGISTRY_START和MGL_PAGE_REGISTRY_END宏来包裹页面注册区域，每一个页面使用MGL_PAGE_ENTRY宏
```c
MGL_PAGE_REGISTRY_START()
    //myPage为页面标识
    MGL_PAGE_ENTRY("myPage",my_page),
MGL_PAGE_REGISTRY_END()
```
::: tip
虽然一个c和h文件就能声明多个页面，甚至一个c文件就能完成多页面的声明和注册，但并不推荐这样做，因为这容易让文件变得复杂，不便维护。
:::


