# Meson 入门

## Quick Start

meson是类似CMake的高阶构建工具, 由python开发；meson可以生成ninja脚本，其中ninja类似make，由C++开发。

当前目录的小项目用法:

```bash
mkdir build
cd build
meson setup ..
meson compile # 或者 ninja
```

或者

```bash
meson setup build && cd build
meson compile
```

## 基本原理

### 1. 配置阶段

运行 `meson setup` 时，Meson 会

1. 解析 `meson.build` 文件：
    - 读取项目的构建目标（如可执行文件、库等）。
    - 解析依赖项、编译选项、安装路径等。
2. 生成 Ninja 构建文件：
    - Meson 不直接调用编译器，而是生成 Ninja 构建文件（build.ninja）。
    - Ninja 是一个高效的构建工具，类似于 `Makefile`，但速度更快。

### 2. 构建阶段

运行 `meson compile` 时，Meson 会调用 Ninja，并传递构建目录中的 `build.ninja` 文件给Ninja：

1. Ninja 读取 `build.ninja` 文件。
2. Ninja 根据规则调用编译器（如 gcc 或 clang）进行编译。

也可以直接调用 `ninja` 工具，ninja是由CPP开发编译的可执行程序。`ninja` 命令会自动读取当前目录下的 `build.ninja` 文件，这是 Ninja 的默认行为。

对于meson自动生成的 `build.ninja` 文件，大概会有如下内容:

```ini
# Rules for compiling.

rule c_COMPILER
 command = cc $ARGS -MD -MQ $out -MF $DEPFILE -o $out -c $in
 deps = gcc
 depfile = $DEPFILE_UNQUOTED
 description = Compiling C object $out

# Rules for linking.

rule c_LINKER
 command = cc $ARGS -o $out $in $LINK_ARGS
 description = Linking target $out
```

## 场景用法

Meson will not allow you to build source code inside your source tree。

- 语法： <https://mesonbuild.com/Syntax.html>
- 目标:  <https://mesonbuild.com/Build-targets.html> 库还是可执行程序
- 头文件搜索内置函数： <https://mesonbuild.com/Include-directories.html>
- 添加参数宏等:  <https://mesonbuild.com/Adding-arguments.html>
- 依赖： <https://mesonbuild.com/Adding-arguments.html>
- 参考手册查内置函数等： <https://mesonbuild.com/Reference-manual.html>

场景的问题:

- <https://mesonbuild.com/FAQ.html#should-i-use-subdir-or-subproject>
  - The answer is almost always subdir. Subproject exists for a very specific use case:
- <https://mesonbuild.com/FAQ.html#why-is-there-not-a-make-backend>
  - Because Make is slow.

## 官方资料

- <https://mesonbuild.com/Reference-manual.html>
- <https://ninja-build.org/manual.html>
- <https://github.com/mesonbuild/meson/tree/master/mesonbuild>
- <https://github.com/ninja-build/ninja/tree/master/src>
