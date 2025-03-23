# 智能指针 `unique_ptr` 原理和用法详解

`unique_ptr` 是 C++11 引入的一种智能指针，用于管理动态分配的对象，其特点是**独占所有权**。以下是关于 `unique_ptr` 的详细说明。

## 实现原理

`std::unique_ptr` 本质上是一个 class 对象，它内部包装了一个 裸指针（即原始指针），并通过封装的方式提供了安全的资源管理功能。一个简化的实现可以看: [简化实现unique_ptr代码](../codes/ptr_unique.cpp). 当在函数中构造一个智能指针 `std::unique_ptr` 时，这个智能指针本质上是一个局部变量对象，它的生命周期由函数的作用域决定。当函数的作用域结束时，智能指针的析构函数会被自动调用，从而释放它所管理的资源。

`std::unique_ptr` 是 C++ 标准库实现，并没有依赖编译器的特殊支持。它的功能完全是通过标准 C++ 语言特性（如模板、RAII、移动语义等）实现的，而不是编译器的内置功能。但是依赖了编译器实现的移动语义，RAII，模板，默认删除器等支持。

## 重载的运算符

`std::unique_ptr` 重载了一些运算符，以便更方便地操作所管理的指针：

- 解引用运算符 `*`：用于访问 `unique_ptr` 所管理的对象，等价于对裸指针的解引用。
- 成员访问运算符 `->`：用于访问 `unique_ptr` 所管理的对象的成员。
- 布尔转换运算符 `operator bool`：用于判断 `unique_ptr` 是否管理一个非空指针，若为空则返回 `false`。

由于 `unique_ptr` 是一个智能指针，不支持指针算术操作（如 `+`、`-`）。`std::unique_ptr` 的设计目标是安全地管理动态分配的资源，而不是完全模拟裸指针的行为。

## 提供的接口

- `get()`：返回 `unique_ptr` 所管理的裸指针，一般不推荐使用，除非有特殊需求。
- `release()`：释放 `unique_ptr` 所管理的指针，并将其所有权转移给调用者，`unique_ptr` 本身变为空。
- `reset()`：替换 `unique_ptr` 所管理的指针；如果当前管理的指针不为空，会先释放旧指针。
- `swap()`：交换两个 `unique_ptr` 所管理的指针。

---

## 使用时的注意点

1. **独占所有权**：`unique_ptr` 不允许多个指针共享同一个对象的所有权。如果需要共享所有权，请使用 `shared_ptr`。
2. **禁止拷贝**：`unique_ptr` 不支持拷贝操作，但可以通过 `std::move` 转移所有权。
3. **自定义删除器**：可以为 `unique_ptr` 指定自定义删除器，用于释放资源。
4. **避免空悬指针**：在转移所有权或重置指针时，确保不会访问已释放的资源。

---

## 使用场景

1. **管理动态分配的资源**：适用于需要动态分配内存并确保资源在离开作用域时自动释放的场景。
2. **避免内存泄漏**：通过 RAII（资源获取即初始化）机制，`unique_ptr` 能有效避免内存泄漏。
3. **临时对象的管理**：适合用于函数返回值或临时对象的管理。

---

## 作用域

`unique_ptr` 的生命周期由其作用域决定。当 `unique_ptr` 离开作用域时，所管理的对象会被自动释放。因此，`unique_ptr` 适用于局部作用域或类的成员变量。

---

## 是否线程安全

`unique_ptr` 本身不是线程安全的。如果需要在多线程环境中使用，建议将其封装在互斥锁中，或者使用线程安全的智能指针（如 `shared_ptr`）。

---

## 示例代码

以下是一些常见的 `unique_ptr` 用法示例：

```cpp
#include <iostream>
#include <memory>
#include <vector>

// 自定义删除器
struct CustomDeleter {
    void operator()(int* ptr) {
        std::cout << "Deleting resource: " << *ptr << std::endl;
        delete ptr;
    }
};

int main() {
    // 1. 基本用法
    std::unique_ptr<int> ptr1(new int(10));
    std::cout << "Value: " << *ptr1 << std::endl;

    // 2. 使用 std::make_unique 创建 unique_ptr（推荐）
    auto ptr2 = std::make_unique<int>(20);
    std::cout << "Value: " << *ptr2 << std::endl;

    // 3. 转移所有权
    std::unique_ptr<int> ptr3 = std::move(ptr2);
    if (!ptr2) {
        std::cout << "ptr2 is now null." << std::endl;
    }
    std::cout << "Value: " << *ptr3 << std::endl;

    // 4. 使用自定义删除器
    std::unique_ptr<int, CustomDeleter> ptr4(new int(30), CustomDeleter());

    // 5. unique_ptr 管理动态数组
    std::unique_ptr<int[]> arr(new int[5]{1, 2, 3, 4, 5});
    for (int i = 0; i < 5; ++i) {
        std::cout << "Array element " << i << ": " << arr[i] << std::endl;
    }

    // 离开作用域时，所有资源会被自动释放
    return 0;
}
```

---

## unique_ptr 修饰类成员变量

`unique_ptr` 非常适合用来管理类的成员变量，尤其是需要动态分配的资源。它可以确保资源在对象销毁时自动释放，避免内存泄漏。

### 示例代码

以下是一个使用 `unique_ptr` 修饰类成员变量的示例：

```cpp
#include <iostream>
#include <memory>
#include <string>

// 当 Class Person 的对象被析构时，它的成员变量也会被自动析构。
//   这是 C++ 的语言特性，成员变量的析构顺序由编译器自动管理。
class Person {
private:
    std::unique_ptr<std::string> name; // 使用 unique_ptr 管理动态分配的字符串
    std::unique_ptr<int> age;          // 使用 unique_ptr 管理动态分配的整数

public:
    // 构造函数
    Person(const std::string& name, int age)
        : name(std::make_unique<std::string>(name)), age(std::make_unique<int>(age)) {}

    // 打印信息
    void printInfo() const {
        std::cout << "Name: " << *name << ", Age: " << *age << std::endl;
    }

    // 修改信息
    void setName(const std::string& newName) {
        *name = newName;
    }

    void setAge(int newAge) {
        *age = newAge;
    }

    // 析构函数自动释放资源
    ~Person() {
        std::cout << "Person destroyed: " << *name << std::endl;
    }
};

int main() {
    // 创建 Person 对象
    Person person("Alice", 30);
    person.printInfo(); // Name: Alice, Age: 30

    // 修改成员变量
    person.setName("Bob");
    person.setAge(35);
    person.printInfo(); // Name: Bob, Age: 35

    // 离开作用域时，资源会自动释放
    // Person destroyed: Bob
    return 0;
}
```

### 注意点

1. **构造函数初始化**：建议使用 `std::make_unique` 初始化成员变量，代码更简洁且安全。
2. **避免裸指针**：不要直接操作 `unique_ptr` 所管理的 **裸指针**，尽量通过智能指针的接口访问资源。
3. **析构函数自动释放**：`unique_ptr` 会在对象 **销毁时自动释放资源** ，无需手动管理。

---

## unique_ptr 修饰全局变量

在某些场景下，可以使用 `unique_ptr` 修饰全局变量来管理动态分配的资源。需要注意的是，全局变量的生命周期贯穿整个程序，因此需要确保资源的正确初始化和释放。

### 示例代码

以下是一个使用 `unique_ptr` 修饰全局变量的示例：

```cpp
#include <iostream>
#include <memory>
#include <string>

// 全局变量使用 unique_ptr
std::unique_ptr<std::string> globalMessage;

void initializeGlobalMessage() {
    globalMessage = std::make_unique<std::string>("Hello, Global unique_ptr!");
}

void printGlobalMessage() {
    if (globalMessage) {
        std::cout << *globalMessage << std::endl;
    } else {
        std::cout << "Global message is not initialized." << std::endl;
    }
}

int main() {
    // 打印未初始化的全局变量
    printGlobalMessage(); // Global message is not initialized.

    // 初始化全局变量
    initializeGlobalMessage();

    // 打印已初始化的全局变量
    printGlobalMessage(); // Hello, Global unique_ptr!

    // 离开作用域时，globalMessage 会自动释放资源
    return 0;
}
```

### 注意点

1. **延迟初始化**：全局变量的初始化可以通过函数延迟执行，避免在程序启动时直接分配资源。
2. **避免裸指针**：全局变量的资源管理应完全交由 `unique_ptr`，不要直接操作裸指针。
3. **自动释放**：程序结束时，`unique_ptr` 会自动释放全局变量所管理的资源，无需手动清理。

---

## 总结

`unique_ptr` 是一种高效且安全的智能指针，适用于需要独占所有权的场景。通过合理使用 `unique_ptr`，可以有效避免内存泄漏和空悬指针问题。在多线程环境中使用时需注意线程安全性。建议：

1. **推荐使用 `std::make_unique`**：在所有动态分配的场景中，优先使用 `std::make_unique`，避免直接使用 `new`。
2. **避免裸指针操作**：尽量通过智能指针的接口访问资源，减少错误风险。
3. **线程安全性说明**：明确指出 `unique_ptr` 本身不是线程安全的，但可以通过外部同步机制（如 `std::mutex`）实现线程安全。
