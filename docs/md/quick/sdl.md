# 快速开始（SDL模拟器）
本篇文件将会讲解如何使用SDL模拟器快速跑通hello示例。
## 前期准备
需要准备的东西如下：
- git
- cmake 3.27+
- python 3
- 编译器

## 克隆仓库
使用如下命令克隆MINGL：
```bash
git clone https://github.com/Tcddm/MINGL.git
```

## 安装SDL
MINGL推荐的SDL版本为2.30.6，如果使用高版本可能导致编译失败。
### Windows
点击[这里](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.6){target="_blank"}访问SDL的仓库，下载其中的`SDL2-devel-2.30.6-mingw.zip`。

然后解压里面的`Makefile`，`x86_64-w64-mingw32`和`cmake`到`platform/sdl/lib`内。

### Linux
执行如下命令安装即可，MINGL会自动识别。
```bash
sudo apt install libsdl2-dev
```

## 下载字体
MINGL默认的字体为MiSans，点击[这里](https://hyperos.mi.com/font/download){target="_blank"}访问并下载，将下载的压缩包里面的`MiSans-Normal.ttf`解压到`tools/font`下

## 配置
首先创建build文件夹并进入：
```bash
mkdir build && cd ./build
```
之后执行：
```bash
cmake ..
```
::: tip 如果cmake ..报错
在Windows中，确认SDL已正确解压到platform/sdl/lib

在Linux中，确认libsdl2-dev已安装
:::
然后就可以执行以下命令开始配置：
::: code-group

```bash [CMake]
cmake --build . --target menuconfig
```

```bash [Make]
make menuconfig
```
:::
因为我们是SDL模拟器加hello示例，所以直接保存默认的配置就行。
::: tip
如果你使用的字体文件名并非`MiSans-Normal.ttf`，则需要修改配置中的`Top->字体生成工具`里的`默认字体文件名`为你使用的字体文件名
:::

## 编译
运行如下命令进行编译：
::: code-group

```bash [CMake]
cmake --build . --target MINGL
```

```bash [Make]
make
```
:::

首次编译会出现无法找到字体文件导致的编译失败，这是因为字体生成工具生成的文件还没生效，再次编译即可。
![编译报错](/img/quick/sdl/error1.png "编译报错")
编译完成如下：
![编译完成](/img/quick/sdl/done.png "编译完成")

## 运行
### Windows
首先需要将`platform/sdl/lib/x86_64-w64-mingw32/bin`下的`SDL2.dll`放在`build`目录下与`MINGL.exe`同级，之后在`build`目录运行`MINGL.exe`即可，运行截图如下：
![Winidows运行截图](/img/quick/sdl/run_win.png "Winidows运行截图")
### Linux
在`build`目录中执行`./MINGL`运行即可，运行截图如下：
![Linux运行截图](/img/quick/sdl/run_linux.png "Linux运行截图")
