# 关于 lambda 的使用场景

C++ 的 Lambda 表达式是一个强大的特性，它不仅限于函数内部使用。本文将深入探讨 lambda 的各种使用场景和最佳实践。

## Lambda 的作用域和声明 (Scope and Declaration)

Lambda 可以在以下作用域中使用：

1. 函数内部 (Function Scope)：最常见的用法。
2. 类成员变量 (Class Member)：作为类的成员存在。
3. 全局作用域 (Global Scope)：在全局范围内定义。
4. 命名空间作用域 (Namespace Scope)：在命名空间中定义。

### 基本语法 (Basic Syntax)

```cpp
auto lambda = [capture-list](parameters) -> return_type { body };
```

* `capture-list`：捕获列表，用于捕获外部变量。
* `parameters`：参数列表。
* `return_type`：返回类型（可选，可以由编译器推导）。
* `body`：函数体。

每个 lambda 表达式的类型是编译器生成的唯一闭包类型。捕获发生的时机实在执行到时，但是行为要关注捕获的是值还是引用。

```cpp
void func() {
    int x = 10;
    auto lambda = [x]() { return x; }; // 捕获发生在执行到此处时
    x = 20;
    std::cout << lambda(); // 输出 10（按值捕获的是创建时的 x 值）
}
```

## 实际使用示例

### 1. 函数内部使用

```cpp
void processData() {
    int multiplier = 10;
    auto multiply = [multiplier](int x) { return x * multiplier; };
    int result = multiply(5); // 结果为 50
}
```

### 2. 类成员变量

lambda可用在类的成员变量中，使用 std::function 类型，不能用auto声明。

在类定义中，成员变量的类型必须在声明时明确指定。这是因为编译器需要在类定义时知道每个成员的大小和布局，而 **auto的类型推导发生在初始化** 时，这会导致不确定性。

* 类定义时，编译器需要知道每个成员变量的类型、大小和偏移量，以计算类的总大小和内存对齐方式。
* 如果允许 auto，类型推导需要依赖初始化表达式，而初始化可能发生在类外的不同位置（尤其是静态成员），编译器无法在类定义时确定类型。

```cpp
#include <functional>

class Calculator {
private:
    int base;
    // 存储为类成员的 lambda
    std::function<int(int)> processor = [this](int x) { return x + base; };

public:
    Calculator(int b) : base(b) {}
    int process(int x) { return processor(x); }
};
```

### 3. 全局作用域 (Global Scope)

全局变量的lambda表达式：如果它是无捕获的，那么可以声明为函数指针类型。但如果有捕获，则必须用std::function。

无捕获的 lambda 可以隐式转换为函数指针，此时允许使用 auto 推导其类型。

```cpp
#include <functional>

// 全局 lambda 需要使用 std::function
std::function<int(int)> globalLambda = [](int x) { return x * 2; };
// same:    int (*globalLambda)(int) = [](int x) { return x * 2; }; // 但是这种依然不建议

void someFunction() {
    int result = globalLambda(5); // 结果为 10
}
```

## 关于类型声明

1. `auto` 声明：

    * 适用于局部变量。
    * 最简单且推荐的方式。
    * 编译器可以准确推导类型 (Type Inference)。

2. `std::function` 声明：

    * 用于类成员或全局变量。
    * 可以存储不同类型的可调用对象 (Callable Objects)。
    * 语法较为冗长。

## 捕获说明

捕获列表的确定发生在**编译期**，但实际值的捕获发生在**运行时**。

常见捕获方式：

* `[]`: 不捕获任何变量。
* `[=]`: 按值捕获所有变量 (Capture by Value)。
* `[&]`: 按引用捕获所有变量 (Capture by Reference)。
* `[this]`: 捕获 `this` 指针。
* `[x, &y]`: 捕获 `x` 的副本，`y` 的引用。

## 最佳实践 (Best Practices)

1. 优先使用 `auto` 进行 lambda 声明。
2. 尽量使用最小捕获范围。
3. 避免在捕获列表中使用 `[=]` 或 `[&]` 这种默认全部捕获的方式，除非必要。
4. 对于小型 lambda，考虑使用隐式返回类型 (Implicit Return Type)，省掉箭头返回值；
5. 需要存储 lambda 时使用 `std::function`。

## 完整示例 (Complete Example)

```cpp
#include <iostream>
#include <functional>

// 全局 lambda (Global lambda)
constexpr auto kGlobalMultiplier = [](int x) noexcept { return x * 3; };

namespace Network {
    // 命名空间作用域 lambda (Namespace-scoped lambda)
    auto packetValidator = [](int packet) { return packet & 0xFF; };
}

class RoboticsController {
private:
    const int m_servoId;
    
    // 类成员 lambda (Class member lambda) - 直接声明初始化
    // 需要捕获 this 时必须使用 std::function
    std::function<int(int)> m_angleCalculator = 
        [this](int input) { 
            return input * m_servoId; // 捕获 this 访问成员
        };

public:
    explicit RoboticsController(int id) : m_servoId(id) {}

    // 返回类型推导示例 (Return type deduction)
    auto getProcessor() {
        // 局部 lambda 使用 auto
        auto localLambda = [*this](int x) { 
            return x + m_servoId;  // 副本捕获当前对象
        };
        return localLambda;
    }

    void execute() const {
        std::cout << "Processed: " << m_angleCalculator(10) << '\n';
    }
};

int main() {
    // 函数作用域 (Function scope)
    auto localLambda = [](auto x) {  // C++14 泛型 lambda
        return x * 2; 
    };
    std::cout << localLambda(5) << '\n';  // 输出 10

    RoboticsController rc(3);
    rc.execute();                         // 输出 30
    
    auto processor = rc.getProcessor();
    std::cout << processor(7) << '\n';    // 输出 10 (3+7)
    
    std::cout << kGlobalMultiplier(4) << '\n'; // 输出 12
    printf("0x%x\n", Network::packetValidator(0xABCD)); // 输出 0xCD
}
```
