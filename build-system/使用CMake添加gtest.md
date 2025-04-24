# CMake添加gtest

## 前言

在C/C++项目开发中，CMake是最广泛使用的构建系统工具之一。本文将介绍如何在不使用`enable_testing()`的情况下，高效组织项目结构和测试框架，特别适合偏好直接运行测试二进制的中小型项目。

## 项目结构设计

### 推荐目录结构

```bash
project/
├── CMakeLists.txt          # 主构建配置
├── src/                    # 主程序源代码
│   ├── main.cpp            # 程序入口
│   └── utils.cpp           # 核心功能模块
└── test/                   # 测试代码
    └── test_utils.cpp      # 单元测试
```

## 核心配置方案

### 精简版CMake配置

根目录CMakeLists.txt**:

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)

# 主程序构建
add_executable(main src/main.cpp src/utils.cpp)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 包含测试目录
add_subdirectory(test)
```

**test/CMakeLists.txt**:

```cmake
# 测试可执行文件构建(也可以父cmake传递过来变量直接用)
add_executable(test_utils test_utils.cpp ../src/utils.cpp)

# 使用find_package引入GTest
find_package(GTest REQUIRED)
target_link_libraries(test_utils PRIVATE GTest::GTest GTest::Main)

# 可选：设置测试文件的运行时路径
set_target_properties(test_utils PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
)
```

## 构建与测试流程

### 标准构建命令

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug  # 生成构建系统
make                              # 编译所有目标
```

### 测试执行方式

```bash
# 直接运行测试程序
./test/test_utils

# 或者如果设置了输出目录
./test/bin/test_utils
```

## 变量作用域管理

### 跨目录变量共享

```cmake
# 根目录CMakeLists.txt
set(SRC_UTILS ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp)
add_subdirectory(test)  # 变量自动对子目录可见

# test/CMakeLists.txt
message("源文件路径: ${SRC_UTILS}")  # 可以正常访问
```

### 文件收集策略

```cmake
# 推荐方式：显式列出
set(MY_SOURCES
    src/main.cpp
    src/utils.cpp
)

# 备选方案：谨慎使用GLOB
file(GLOB SRC_FILES LIST_DIRECTORIES false 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
```

## 测试框架集成

### Google Test集成

```cmake
# test/CMakeLists.txt
find_package(GTest REQUIRED)

# 测试目标配置
add_executable(test_utils test_utils.cpp ${SRC_UTILS})
target_link_libraries(test_utils PRIVATE 
    GTest::GTest 
    GTest::Main
    Threads::Threads  # 如果需要多线程支持
)
```

## 高级技巧

### 条件编译控制

```cmake
option(BUILD_TESTS "是否构建测试" ON)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
```

用法：

| 场景                 | 写法                                      |
|----------------------|------------------------------------------|
| 定义可配置开关       | `option(OPT_NAME "描述" DEFAULT_VALUE)`  |
| 命令行启用选项       | `cmake -DOPT_NAME=ON`                    |
| 命令行禁用选项       | `cmake -DOPT_NAME=OFF`                   |
| 条件控制代码逻辑     | `if(OPT_NAME) ... endif()`               |

### 自定义构建目标

```cmake
# 添加运行测试的自定义目标
add_custom_target(run_tests
    COMMAND ./test/test_utils
    COMMENT "Running tests..."
    VERBATIM
)
```

## 常见问题解决方案

### 问题1：测试程序找不到共享库

解决方案：

```cmake
# 设置RPATH（相对路径）
set_target_properties(test_utils PROPERTIES
    BUILD_RPATH "$ORIGIN/../lib"
    INSTALL_RPATH "$ORIGIN/../lib"
)
```

### 问题2：需要测试私有类成员

推荐做法：

1. 将被测类声明和实现分离
2. 在测试代码中包含实现文件：

```cpp
#include "../src/class_impl.cpp"  // 直接包含实现
```

### 问题3：多测试目标管理

```cmake
# 在test/目录中
foreach(test_file IN ITEMS test1.cpp test2.cpp)
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file} ${SRC_UTILS})
    target_link_libraries(${test_name} PRIVATE GTest::GTest)
endforeach()
```

## 结语

本文介绍的精简版CMake配置方案具有以下优势：

1. 避免不必要的CTest配置
2. 保持构建系统的简洁性
3. 提供直接控制测试执行的能力
4. 维护变量作用域的清晰性

适合场景：

- 中小型项目
- 需要快速迭代的开发环境
- 偏好直接运行测试的团队

记住CMake的核心原则：保持简单、明确和可维护。根据项目规模的增长，可以随时引入更高级的测试管理功能。
