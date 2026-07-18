# 图形化配置
MINGL引入了Kconfiglib实现图形化配置和转义生成配置头文件
## 使用
menuconfig已经被添加到cmake的target中，使用方式如下：
::: code-group

```bash [CMake]
cmake --build build --target menuconfig
```

```bash [Make]
make menuconfig
```

```bash [Python]
python tools/kconfig.py menuconfig
```

:::

![图形化配置界面](/img/menuconfig.png "图形化配置界面")

## 配置生效流程
```
menuconfig TUI → 写入 .config → CMake 读取 → 注入 -DCONFIG_XX=value
  → kconfig.py genheader → mgl_config_gen.h → 编译生效
```

## 添加新平台
- Kconfig — choice 块中加 config MGL_PLATFORM_XXX
- Kconfig — 末尾加 source "platform/xxx/Kconfig"
- platform/xxx/Kconfig — 平台特有配置
- platform/xxx/CMakeLists.txt — 编译配置
## 添加新配置项
```
Kconfig 中加 config MGL_XXX 定义
mgl_config.h 中通过 #include "mgl_config_gen.h" 自动获得宏定义
CMake 零改动
```