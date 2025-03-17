# 右值和移动语义

[book: 右值的相关定义](../book/./03-runtime.md#33-右值引用)

---

## 1. **左值、右值与右值引用的引入**

- **C++98/03**: 只有左值和右值，不能区分 **可修改的左值（modifiable lvalue）** 和 **不可修改的左值（const lvalue）**。
- **C++11**: 引入 **右值引用（rvalue reference, `T&&`）**，使得可以区分左值和右值，并支持 **移动语义（move semantics）** 和 **完美转发（perfect forwarding）**。
- **C++14**: 引入 **泛左值引用（universal reference）**，即 `T&&` 在模板参数推导时的特殊情况。
- **C++17**: 引入 **std::invoke、std::apply** 等工具，增强了完美转发在泛型编程中的应用。

---

## 2. **作用**

### **(1) 右值引用 (`T&&`) 的作用**

- **支持移动语义**: **避免深拷贝**，提高性能。例如 `std::move` 可将对象转换为右值，从而调用 `T&&` 版本的构造函数或赋值运算符。
- **实现完美转发**: 允许 **泛型** 代码在函数 **调用时保持参数的原始值** 类别（左值/右值）。

### **(2) 完美转发的作用**

- **避免不必要的拷贝**: 通过 `std::forward<T>(arg)` 保留参数的左值或右值特性，提高效率。
- **增强泛型编程能力**: 允许模板函数能够适用于所有类型的参数，而不会意外丢失值类别信息。

---

## 3. **原理**

### **(1) 左值和右值**

- **左值（lvalue）**: 可取地址的对象，例如变量、返回左值的函数调用。
- **右值（rvalue）**: 不能取地址的临时对象，例如 `5`、`std::string("hello")`，或返回右值的函数调用。

**示例**：

```cpp
int x = 10;         // x 是左值
int& lref = x;      // 左值引用
int&& rref = 10;    // 右值引用，仅能绑定右值
```

### **(2) 右值引用和移动语义**

```cpp
class MyClass {
public:
    std::vector<int> data;
    
    // 复制构造函数（拷贝）
    MyClass(const MyClass& other) : data(other.data) {}

    // 移动构造函数（避免拷贝）
    MyClass(MyClass&& other) noexcept : data(std::move(other.data)) {}
};
```

- `std::move(other.data)` 将 `other.data` 变成右值，调用 `std::vector` 的 **移动构造函数**，避免不必要的拷贝，提高性能。

​移动构造函数直接「窃取」右值对象的资源（data 指针），提高性能。举例如下：

```cpp
#include <iostream>
#include <cstring>
#include <utility>  // std::move

class MyString {
    char* data;
    size_t length;

public:
    // 普通构造函数
    MyString(const char* str = "") {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    }

    // 析构函数
    ~MyString() {
        delete[] data;
    }

    // 拷贝构造函数（深拷贝）
    MyString(const MyString& other) {
        length = other.length;
        data = new char[length + 1];
        strcpy(data, other.data);
    }

    // 移动构造函数（窃取资源）
    MyString(MyString&& other) noexcept {
        data = other.data;     // 接管指针
        length = other.length;
        other.data = nullptr;  // 置空原指针
        other.length = 0;
    }

    // 移动赋值运算符, 释放当前资源, 窃取移动赋值资源
    MyString& operator=(MyString&& other) noexcept {
        if (this != &other) {
            delete[] data;         // 释放当前资源
            data = other.data;     // 接管新指针
            length = other.length;
            other.data = nullptr;  // 置空原指针
            other.length = 0;
        }
        return *this;
    }

    // 打印内容
    void print() const {
        std::cout << (data ? data : "nullptr") << "\n";
    }
};

int main() {
    MyString s1 = "Hello";  // 普通构造
    MyString s2 = std::move(s1);  // 移动构造, s1转换成右值, 触发右值构造函数

    std::cout << "s1: ";
    s1.print();  // 输出 nullptr（资源已被转移）

    std::cout << "s2: ";
    s2.print();  // 输出 Hello

    MyString s3;
    s3 = std::move(s2);  // 移动赋值

    std::cout << "s2: ";
    s2.print();  // 输出 nullptr
    std::cout << "s3: ";
    s3.print();  // 输出 Hello
}
```

### **(3) 完美转发 (`std::forward`)**

- `std::forward<T>(arg)` 仅在 `T` 是 **右值引用** 时，将 `arg` 作为右值传递，否则仍然作为左值传递。
- 主要 **用于泛型** 代码，使模板函数能够保留参数的原始值类别。

**示例**：

```cpp
#include <iostream>
#include <utility>

void overloaded(int& x) { std::cout << "Lvalue\n"; }
void overloaded(int&& x) { std::cout << "Rvalue\n"; }

template <typename T>
void forwarding(T&& arg) {
    overloaded(std::forward<T>(arg));  // 关键：保持值类别
}

int main() {
    int x = 5;
    forwarding(x);    // Lvalue
    forwarding(10);   // Rvalue
}
```

- `T&&` 在 `forwarding(x)` 时，`T` 推导为 `int&`，`arg` 变成 `int&`，调用 `overloaded(int&)`。
- `T&&` 在 `forwarding(10)` 时，`T` 推导为 `int`，`arg` 变成 `int&&`，调用 `overloaded(int&&)`。

---

## 4. **最佳实践**

### **(1) 使用 `std::move` 进行移动**

- 仅当对象不会再被使用时，使用 `std::move` 将其转换为右值，以避免不必要的拷贝。

```cpp
std::vector<int> foo() {
    std::vector<int> v = {1, 2, 3};
    return std::move(v);  // 不推荐，因为 RVO 优化已经避免拷贝
}
```

- **推荐**:

```cpp
std::vector<int> foo() {
    return {1, 2, 3};  // RVO (Return Value Optimization)
}
```

### **(2) 使用 `std::forward` 进行完美转发**

- 在泛型代码中，确保保留参数的原始值类别。

```cpp
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
```

### **(3) 避免 `std::forward` 误用**

- 仅在泛型代码中使用 `std::forward`，不要在普通函数中使用 `std::forward` 代替 `std::move`。

**错误示例**：

```cpp
void func(std::string&& str) {
    std::string new_str = std::forward<std::string>(str);  // ❌ 误用
}
```

- 这里 `str` 已经是右值引用，应使用 `std::move(str)`。

---

## 5. **总结**

| 版本   | 关键特性               | 作用                  |
|--------|--------------------|--------------------|
| C++98  | 左值、右值           | 经典值类别            |
| C++11  | 右值引用 `T&&`、`std::move`、`std::forward` | 移动语义、完美转发 |
| C++14  | 泛左值引用 `T&&`        | 泛型编程            |
| C++17  | `std::invoke` 等工具   | 增强泛型能力          |

**最佳实践总结**：
✅ 使用 `std::move` 进行移动，但避免 RVO 场景中误用。  
✅ 在泛型代码中，使用 `std::forward<T>` 进行完美转发。  
✅ 避免 `std::forward` 误用，应仅在模板代码中使用。

你可以在 C++ 高效编程（如 `std::vector` 的移动优化）或泛型库开发（如 `std::make_unique`）中利用这些特性，以提升性能和代码复用性。

## **示例：返回一个复杂的结构体**

```cpp
#include <iostream>
#include <vector>

struct BigStruct {
    std::vector<int> data;
    
    BigStruct(size_t n) : data(n, 42) {
        std::cout << "Constructor\n";
    }
    
    BigStruct(const BigStruct& other) {
        std::cout << "Copy Constructor\n";
    }

    BigStruct(BigStruct&& other) noexcept {
        std::cout << "Move Constructor\n";
    }
};

BigStruct createBigStruct() {
    BigStruct b(1000);  // 栈上的临时变量
    return b;  // RVO: 编译器会直接在调用者的栈上构造 b，避免拷贝/移动
}

int main() {
    BigStruct result = createBigStruct();  // 触发 RVO
}
```

- **没有调用拷贝/移动构造函数**，意味着编译器直接在 `result` 的栈上构造 `BigStruct`（RVO）。
- **如果编译器没有进行 RVO**，那么 `BigStruct` 仍然是一个右值，将触发移动构造（`BigStruct(BigStruct&&)`）。

**错误写法**（不推荐）：

```cpp
BigStruct createBigStruct() {
    BigStruct b(1000);
    return std::move(b);  // ❌ 反而可能禁用 RVO
}
```

- **问题**：
  - `std::move(b)` **会把 `b` 变成右值**，但这样会告诉编译器 **不能使用 RVO**，导致必须调用**移动构造**。
  - 反而可能引入额外的性能开销。

**正确写法**：

```cpp
BigStruct createBigStruct() {
    return BigStruct(1000);  // ✅ 编译器可直接优化
}
```

- **推荐写法**：让编译器直接优化，而不要手动 `std::move` 影响 RVO。

---

### **4. 什么时候 `std::move` 才有用？**

如果**返回值是函数参数**或**成员变量**，**RVO 不适用**，才需要 `std::move`：

```cpp
BigStruct passThrough(BigStruct b) {
    return std::move(b);  // ✅ 这里 `b` 是命名变量，无法 RVO，手动 std::move
}
```

**但如果你自己创建的临时变量，就不要 `std::move`**。

---

### **5. 结论**

| 场景 | **推荐方式** | **是否需要 `std::move`？** |
|------|-------------|---------------------|
| **返回局部变量** | `return myStruct;` | ❌（编译器可优化） |
| **返回临时对象** | `return MyStruct(...);` | ❌（直接 RVO） |
| **返回函数参数** | `return std::move(arg);` | ✅（避免拷贝） |
| **返回类成员** | `return std::move(this->member);` | ✅（避免拷贝） |

✅ **结论**：

- **大结构体临时变量适合右值返回**。
- **C++17 之后会强制 RVO**，不用 `std::move` 让编译器优化。
- **只有在返回参数或成员变量时才需要 `std::move`**。
