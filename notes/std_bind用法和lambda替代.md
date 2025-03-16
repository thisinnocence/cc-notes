# std::bind 使用指南

## 1. std::bind 简介

`std::bind` 是 C++ 标准库 `<functional>` 头文件中的一个函数适配器，用于创建新的可调用对象。它能够将函数及其部分参数绑定到一起，使得调用时无需再提供这些参数。

### 主要功能

- **固定可调用对象的部分参数**，简化调用时的参数传递。
- **改变参数的调用顺序**，提供灵活的参数排列方式。
- **将成员函数转换为可调用对象**，便于存储和使用。

## 2. 基本语法

```cpp
#include <functional>

auto new_func = std::bind(callable, args...);
```

### 参数绑定说明

- `std::placeholders::_1, _2, ...` 代表新函数的第 1、2、… 个参数。
- 直接传递的值会被固定，调用时无需再提供。
- **成员函数绑定格式**：

  ```cpp
  std::bind(&Class::method, instance_ptr, args...);
  ```

## 3. 示例代码

### 3.1 绑定普通函数

```cpp
#include <iostream>
#include <functional>

int add(int a, int b) {
    return a + b;
}

int main() {
    auto add_five = std::bind(add, 5, std::placeholders::_1);
    std::cout << "add_five(10) = " << add_five(10) << std::endl; // 输出 15
    return 0;
}
```

### 3.2 绑定成员函数

```cpp
#include <iostream>
#include <functional>

class Calculator {
public:
    int multiply(int x, int y) {
        return x * y;
    }
};

int main() {
    Calculator calc;
    auto multiply_by_two = std::bind(&Calculator::multiply, &calc, std::placeholders::_1, 2);
    std::cout << "multiply_by_two(4) = " << multiply_by_two(1) << std::endl; // 输出 2
    return 0;
}
```

### 3.3 结合 `std::function` 存储可调用对象

```cpp
#include <iostream>
#include <functional>

int add(int a, int b) {
    return a + b;
}

class Calculator {
public:
    int multiply(int x, int y) {
        return x * y;
    }
};

int main() {
    std::function<int(int)> func_array[2];

    func_array[0] = std::bind(add, std::placeholders::_1, 20);
    Calculator calc;
    func_array[1] = std::bind(&Calculator::multiply, &calc, std::placeholders::_1, 2);

    std::cout << "func_array[0](5) = " << func_array[0](5) << std::endl;   // 输出 25
    std::cout << "func_array[1](1)  = " << func_array[1](1) << std::endl;  // 输出 2

    return 0;
}
```

## 4. `std::bind` 与 `lambda`

在现代 C++ 中，`std::bind` 可以被 **Lambda 表达式** 替代。例如：

```cpp
// 使用 std::bind
auto add_five = std::bind(add, 5, std::placeholders::_1);

// 等价的 Lambda
auto add_five_lambda = [](int x) { return add(5, x); };
```

`lambda` 语法更直观且无须引入 `std::placeholders`，推荐在 C++11 及以上版本中优先使用 `lambda`。

### 4.1 改造前面例子的 `std::bind`

#### 绑定普通函数的 Lambda 替代方案

```cpp
#include <iostream>

int add(int a, int b) {
    return a + b;
}

int main() {
    auto add_five_lambda = [](int x) { return add(5, x); };
    std::cout << "add_five_lambda(10) = " << add_five_lambda(10) << std::endl; // 输出 15
    return 0;
}
```

#### 绑定成员函数的 Lambda 替代方案

```cpp
#include <iostream>

class Calculator {
public:
    int multiply(int x, int y) {
        return x * y;
    }
};

int main() {
    Calculator calc;
    auto multiply_by_two_lambda = [&calc](int x) { return calc.multiply(x, 2); };
    std::cout << "multiply_by_two_lambda(4) = " << multiply_by_two_lambda(1) << std::endl; // 输出 2
    return 0;
}
```

#### 结合 `std::function` 的 Lambda 替代方案

```cpp
#include <iostream>
#include <functional>

int add(int a, int b) {
    return a + b;
}

class Calculator {
public:
    int multiply(int x, int y) {
        return x * y;
    }
};

int main() {
    std::function<int(int)> func_array[2];
    
    func_array[0] = [](int x) { return add(x, 20); };
    Calculator calc;
    func_array[1] = [&calc](int x) { return calc.multiply(x, 2); };

    std::cout << "func_array[0](5) = " << func_array[0](5)  << std::endl;  // 输出 25
    std::cout << "func_array[1](1) = " << func_array[1](1)  << std::endl;  // 输出 2

    return 0;
}
```

## 5. `std::bind` 适用场景

- 需要 **预绑定部分参数** 的函数对象。
- 需要 **改变参数顺序** 时。
- 需要将 **成员函数转换为可调用对象** 时。
- 结合 `std::function` 进行 **回调函数管理**。

## 6. 结论

`std::bind` 是一个强大的工具，在 C++11 及以上版本中仍然有其作用，尤其在函数对象管理和参数适配上。但在现代 C++ 代码中，Lambda 表达式通常是更优雅的替代方案。
