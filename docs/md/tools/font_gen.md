# 字体生成工具
为了方便开发，MINGL提供了字体生成工具，目前支持1bpp位图字体
## 使用
:::warning
由于框架并不会每次编译都生成字体，只会在工具本身或.config变化才会自动重新生成，所以每次添加新字都要手动执行
:::
fonts已经被添加到cmake的target中，使用方式如下：
::: code-group

```bash [CMake]
cmake --build build --target fonts
```

```bash [Make]
make fonts
```

```bash [Python]
python tools/font_gen.py
python tools/font_gen.py --scan-path demo/hello  #指定扫描路径
```

:::

## 便捷宏
宏|用途|示例
-|-|-
MGL_STR(s)|默认字号的静态文本|MGL_STR("OK")
MGL_STR_SIZE(s,n)|指定字号的静态文本|MGL_STR_SIZE("Title", 24)
MGL_FMT(fmt)|sprintf格式字符串，含数字|MGL_FMT("Count: %d",20)
MGL_FMT_SIZE(fmt,n)|指定字号的格式字符串|MGL_FMT_SIZE("%.1f°C",20)

使用MGL_FMT/MGL_FMT_SIZE会自动追加0-9

以上的宏实际上为标记使用到的文本，STR宏同时代为指定字体，比如MGL_STR_SIZE("Send",24)展开为`"Send",.font=&mgl_font_24px`

## 生成产物
脚本在 generated/font/ 下生成

## 字符集管理
::: tip 不需要手动维护字符集 
工具扫描源码中所有 MGL_STR / MGL_FMT 宏，自动提取字符并去重。

新增一行 MGL_STR("✓",16) → 脚本自动加入 ✓ 字符 → 生成 16px 的 ✓ 字形。不需要编辑配置表。 
:::

## 添加新字号
- 在代码中用 MGL_STR_SIZE("text",size) 引用新字号
- 运行工具
- CMake GLOB 自动收集新的 .c 文件 → 编译生效

## 配置
通过Kconfig配置工具参数，注意如果启用了demo，则不会显示`源码扫描路径`，而是自动采用`demo/xxx`
