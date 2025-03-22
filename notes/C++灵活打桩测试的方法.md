# C/C++ 单元测试核心技巧：灵活打桩

单元测试的核心目标是隔离待测代码，确保其逻辑可以被充分验证。在 C/C++ 这种静态类型语言中，测试的一个关键点就是 **灵活打桩（Mocking/Stubbing）**，即在测试环境下替换或拦截函数调用，以实现对目标代码的精细化控制。本文将从多种技术手段出发，系统总结 C/C++ 单元测试中的打桩方法。

---

## 1. 直接 `#include` 被测 `.c/.cpp` 文件

通常，在 C/C++ 项目中，源文件（ `.c/.cpp` ）和头文件（ `.h/.hpp` ）是分开的，测试代码一般只 `#include` 头文件。但实际上，我们可以 **直接包含源文件**，这样测试代码就能访问其所有函数，包括 `static` 函数。

### **示例**

```c
// source.c
static int hidden_function() {
    return 42;
}
```

```c
// test.c
#include "source.c"  // 直接包含源码文件
#include <assert.h>

int main() {
    assert(hidden_function() == 42); // 直接调用 static 函数
    return 0;
}
```

### **优点**

* 允许直接测试 `static` 函数，无需修改被测代码。
* 代码可见性高，适用于白盒测试。

### **缺点**

* 可能导致多个 `.c` 文件重复定义同一符号，影响链接。
* 不适用于大规模项目，因其破坏模块化。

---

## 2. 定义同名函数覆盖依赖项

C 语言的 **符号解析规则** 允许在链接时，后定义的符号覆盖前面的，因此可以在测试代码中定义同名函数来覆盖库函数或依赖项。

### **示例：覆盖 `malloc` 以模拟分配失败**

```c
#include <stdlib.h>
#include <stdio.h>

void* malloc(size_t size) {
    return NULL;  // 强制让 malloc 失败
}

int main() {
    void* ptr = malloc(100);
    if (ptr == NULL) {
        printf("Mocked malloc failed as expected!\n");
    }
    return 0;
}
```

### **优点**

* 适用于拦截系统库函数或外部依赖。
* 代码修改最小化，无需改动待测代码。

### **缺点**

* 仅适用于链接级别的替换，若静态链接，可能无效。
* 可能导致未定义行为，特别是多线程程序。

---

## 3. 宏替换（ `#define` ）

利用 C 预处理器宏，可以用 `#define` 替换函数调用，常用于 **替换系统调用或第三方库函数**。

### **示例：拦截 `printf` **

```c
#include <stdio.h>
#define printf(...)  mock_printf(__VA_ARGS__)

void mock_printf(const char* format, ...) {
    // 自定义日志输出，或者记录日志到缓冲区
}
```

### **优点**

* 适用于封装好的 API 层测试。
* 可用于测试代码中的 `printf`、`malloc` 等常见系统调用。

### **缺点**

* 仅限于宏展开的作用范围。
* 可能影响其他头文件，导致难以调试。

---

## 4. `setjmp/longjmp` 实现异常分支测试

C 语言没有异常处理机制，但可以利用 `setjmp/longjmp` **模拟异常返回**，用于测试错误路径。这在单元测试中特别有用，尤其是测试系统调用失败、资源分配失败等异常情况。

### **基础示例**

```c
#include <setjmp.h>
#include <stdio.h>

jmp_buf env;

void error_function() {
    longjmp(env, 1); // 模拟错误跳转
}

int main() {
    if (setjmp(env) == 0) {
        error_function();
        printf("This line should not execute\n");
    } else {
        printf("Caught error!\n");
    }
    return 0;
}
```

### **实际测试应用示例：文件操作测试**

```c
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <assert.h>

// 全局跳转缓冲区
static jmp_buf test_env;
static int should_fail = 0;

// 模拟的文件操作函数
FILE* my_fopen(const char* filename, const char* mode) {
    if (should_fail) {
        longjmp(test_env, 1);  // 模拟打开文件失败
    }
    return fopen(filename, mode);
}

// 被测试的实际业务函数
int process_file(const char* filename) {
    FILE* file = my_fopen(filename, "r");
    if (!file) {
        return -1;
    }
    // ... 处理文件 ...
    fclose(file);
    return 0;
}

// 测试用例
void test_file_operation() {
    // 测试正常路径
    should_fail = 0;
    if (setjmp(test_env) == 0) {
        int result = process_file("test.txt");
        assert(result == 0);
        printf("Normal path test passed\n");
    }

    // 测试失败路径
    should_fail = 1;
    if (setjmp(test_env) == 0) {
        process_file("test.txt");
        printf("This line should not execute\n");
    } else {
        printf("File operation failure test passed\n");
    }
}
```

### **适用场景**

* 测试系统调用失败情况
* 模拟资源分配失败
* 测试 I/O 操作异常
* 测试深层调用栈中的错误处理
* 验证错误恢复机制

### **优点**

* 适用于测试 `abort()`、`exit()` 这类终止流程的函数
* 可用于模拟异常路径，提高测试覆盖率
* 能够精确控制错误发生的时机和位置
* 无需修改被测试代码的结构

### **缺点**

* `longjmp` 可能影响代码可读性和可维护性
* 不能用于 C++，因其不支持对象析构
* 局部变量在 `longjmp` 后的值可能不确定
* 需要小心处理资源清理

---

## 5. 使用 Mock 框架进行测试

Mock 框架用于 **创建模拟对象**，替换真实依赖，以便更精确地测试单元逻辑。

### **Mock 的原理**

* **接口替换**：使用 C++ 的虚函数或 C 语言的函数指针来替换真实依赖。
* **动态拦截**：通过 `LD_PRELOAD` 或 `--wrap` 等技术替换库函数。
* **手工 Mock**：手动提供替代实现，以返回预设值或模拟特定行为。

### **常见 Mock 库**

* **Google Mock**（C++）：与 Google Test 配合使用，支持丰富的 Mock 规则。
* **CMock**（C）：基于 `Ceedling`，用于 C 语言的函数 Mock。
* **Fake Function Framework（fff）**：适用于 C 语言，基于 `#define` 生成 Mock。
* **Trompeloeil**（C++）：支持现代 C++，基于模板和 lambda。

### **示例：使用 Google Mock**

```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Database {
public:
    virtual int get_data() = 0;
};

class MockDatabase : public Database {
public:
    MOCK_METHOD(int, get_data, (), (override));
};

TEST(MyTest, TestMock) {
    MockDatabase mock_db;
    EXPECT_CALL(mock_db, get_data()).WillOnce(::testing::Return(42));
    ASSERT_EQ(mock_db.get_data(), 42);
}
```

### **优点**

* 适用于复杂的依赖管理，支持精确的行为控制。
* 适合大型项目的单元测试，提高测试覆盖率。

### **缺点**

* 需要额外的框架，学习成本较高。
* 可能导致测试代码比业务代码更复杂。

---

## 总结

| 方法             | 适用范围         | 优缺点                     |
| ---------------- | ---------------- | -------------------------- |
| `#include .c` | 测试 static 函数 | 破坏模块化                 |
| 同名函数覆盖     | 覆盖依赖         | 链接替换，不适用于静态链接 |
| `#define` 宏替换 | 替换库函数       | 影响范围大，调试难         |
| `setjmp/longjmp` | 测试异常路径     | 代码可读性低               |
| `LD_PRELOAD` | 劫持动态库       | 仅适用于 Linux             |
| `--wrap` 选项    | 链接时替换       | 需要编译时指定             |
| Mock 框架        | 复杂依赖管理     | 适用于大型项目             |

根据具体场景选择合适的打桩方法，提升 C/C++ 单元测试的灵活性和可测性！
