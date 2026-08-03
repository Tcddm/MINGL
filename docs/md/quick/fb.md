# 快速开始（Linux FrameBuffer）
本篇文件将会讲解如何使用Linux FrameBuffer快速跑通hello示例。

## 指定交叉编译器
::: warning 注意 GCC 版本
MINGL依赖GCC的`复合字面量+静态初始化器`扩展特性

- **最低技术要求**：GCC 5.2（修复了 `-pedantic-errors` 下的误报）
- **强烈推荐**：GCC 8.3+（如 Linaro 8.3）或 ARM GNU Toolchain 10+
- **不推荐**：GCC 4.x/5.0/5.1（在严格编译模式下会直接报错）

框架测试使用的交叉编译器为arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-linux-gnueabihf
:::
进入到`build`目录，使用如下命令指定交叉编译器：
```bash
cmake .. \
    -DCMAKE_C_COMPILER=arm-none-linux-gnueabihf-gcc
```
如果目标板子的`glibc`版本不符合且不方便升级glibc的可以使用静态编译：
```bash
cmake .. \
    -DCMAKE_C_COMPILER=arm-none-linux-gnueabihf-gcc \
    -DCMAKE_EXE_LINKER_FLAGS="-static" 
```
