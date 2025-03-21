# C++17 模板编程入门

C++ 模板（Template）是泛型编程的核心工具，它允许我们编写更通用、可复用的代码。对于 C++17 及以前的版本，模板主要用于 **函数模板** 和 **类模板**，以及一些更高级的特性如 **可变参数模板** 和 **模板特化**。

本篇博客将帮助你入门 C++ 模板编程，重点介绍：

1. **函数模板**
2. **类模板**
3. **模板特化**
4. **可变参数模板**
5. **C++17 相关特性**

每个部分都会配有示例代码，并附上详细的中文注释，方便理解。

---

## 1. 函数模板（Function Template）

函数模板允许我们编写与类型无关的通用函数，在编译时由具体类型实例化。

### 示例：交换两个变量

```cpp
#include <iostream>

// 定义一个通用的 swap 函数
template <typename T>
void mySwap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

int main() {
    int x = 10, y = 20;
    mySwap(x, y);  // 这里 T 被推导为 int
    std::cout << "x = " << x << ", y = " << y << std::endl;

    double a = 1.5, b = 2.5;
    mySwap(a, b);  // 这里 T 被推导为 double
    std::cout << "a = " << a << ", b = " << b << std::endl;

    return 0;
}
```

**说明：**

- `template <typename T>` 定义了一个模板，`T` 是一个占位符。
- `mySwap(T& a, T& b)` 在调用时，会根据实参的类型推导 `T`，无需显式指定。

---

## 2. 类模板（Class Template）

类模板允许我们定义泛型类，在编译时根据不同类型进行实例化。

### 示例：通用的数组包装器

```cpp
#include <iostream>

template <typename T, size_t N>
class Array {
private:
    T data[N];  // 使用模板参数 T 和 N 定义数组

public:
    // 赋值运算
    void set(size_t index, T value) {
        if (index < N) {
            data[index] = value;
        }
    }

    // 读取运算
    T get(size_t index) const {
        return (index < N) ? data[index] : T();
    }

    // 获取数组大小
    size_t size() const { return N; }
};

int main() {
    Array<int, 5> intArray;  // 创建一个存储 5 个 int 的数组
    intArray.set(0, 42);
    std::cout << "First element: " << intArray.get(0) << std::endl;

    Array<double, 3> doubleArray;  // 创建一个存储 3 个 double 的数组
    doubleArray.set(1, 3.14);
    std::cout << "Second element: " << doubleArray.get(1) << std::endl;

    return 0;
}
```

**说明：**

- `template <typename T, size_t N>` 允许类接受两个模板参数。
- `data[N]` 使数组的大小 `N` 变成编译时常量，避免动态分配内存。

---

## 3. 模板特化（Template Specialization）

模板特化允许我们为特定类型提供不同的实现方式。

### 示例：bool 类型的特化

```cpp
#include <iostream>

// 普通模板
template <typename T>
class TypeInfo {
public:
    static void print() {
        std::cout << "General type\n";
    }
};

// 针对 bool 类型的特化
template <>
class TypeInfo<bool> {
public:
    static void print() {
        std::cout << "Boolean type\n";
    }
};

int main() {
    TypeInfo<int>::print();  // 输出 General type
    TypeInfo<bool>::print(); // 输出 Boolean type
    return 0;
}
```

**说明：**

- `template <> class TypeInfo<bool>` 是一个针对 `bool` 类型的特化版本。
- 这样，我们可以对不同类型提供特定实现。

---

## 4. 可变参数模板（Variadic Templates）

可变参数模板可以接受多个类型参数，在 C++11 之后引入，是泛型编程的重要增强。

### 示例：递归展开的可变参数打印函数

```cpp
#include <iostream>

// 递归终止条件：只有一个参数时直接打印
template <typename T>
void printArgs(T arg) {
    std::cout << arg << std::endl;
}

// 递归展开
template <typename T, typename... Args>
void printArgs(T first, Args... rest) {
    std::cout << first << " ";
    printArgs(rest...);  // 递归调用展开剩余参数
}

int main() {
    printArgs(1, 2.5, "Hello", 'A');  
    return 0;
}
```

**说明：**

- `typename... Args` 允许 `printArgs` 接受可变数量的参数。
- `printArgs(first, rest...)` 通过递归方式展开参数包。

---

## 5. C++17 相关模板特性

C++17 引入了一些增强模板编程的特性，比如 **`if constexpr`** 和 **折叠表达式（Fold Expressions）**。

### 5.1 `if constexpr`（编译期分支）

`if constexpr` 允许在编译期进行条件判断，避免无效代码实例化。

```cpp
#include <iostream>

// 泛型加法函数
template <typename T>
void printValue(T value) {
    if constexpr (std::is_integral<T>::value) {
        std::cout << "整数: " << value << std::endl;
    } else {
        std::cout << "浮点数: " << value << std::endl;
    }
}

int main() {
    printValue(10);    // 输出：整数: 10
    printValue(3.14);  // 输出：浮点数: 3.14
    return 0;
}
```

**说明：**

- `if constexpr` 让编译器只实例化符合条件的代码，避免无效代码带来的编译错误。

### 5.2 折叠表达式（Fold Expressions）

可变参数模板的一种新特性，简化递归展开的写法。

```cpp
#include <iostream>

// C++17 折叠表达式
template <typename... Args>
auto sum(Args... args) {
    return (args + ...);  // 使用折叠表达式计算总和
}

int main() {
    std::cout << sum(1, 2, 3, 4, 5) << std::endl;  // 输出 15
    return 0;
}
```

**说明：**

- `(args + ...)` 是 **右折叠**，即 `((1 + 2) + 3) + 4) + 5`。
- 这种写法比递归展开更简洁。

---

## 结语

本篇博客介绍了 C++17 模板编程的核心基础，涵盖：

- **函数模板**（泛型函数）
- **类模板**（泛型类）
- **模板特化**（特定类型的优化）
- **可变参数模板**（支持多个参数）
- **C++17 新特性**（`if constexpr` 和折叠表达式）

模板编程是 C++ 的强大特性，熟练掌握后能大幅提高代码的复用性和灵活性。建议进一步学习 **SFINAE、Concepts（C++20）、模板元编程（TMP）** 等更高级的内容。

希望这篇博客对你有所帮助！ 🎯
