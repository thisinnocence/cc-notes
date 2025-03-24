# 智能指针 `shared_ptr` 和 `weak_ptr` 用法

C++11 引入了 `shared_ptr` 和 `weak_ptr`，它们是标准库中提供的智能指针，用于安全地管理动态分配的资源。以下是关于它们的详细说明。

---

## 基本实现原理

`shared_ptr` 和 `weak_ptr` 是由 C++ 标准库实现的，而非编译器直接支持。它们依赖于引用计数机制来管理资源的生命周期：

1. **引用计数**：`shared_ptr` 内部维护一个引用计数（`use_count`），记录有多少个 `shared_ptr` 实例共享同一个资源。当最后一个 `shared_ptr` 被销毁时，引用计数归零，资源会被自动释放。
2. **弱引用**：`weak_ptr` 不会增加引用计数，它仅仅是对资源的一个非拥有型引用，用于解决循环引用问题。

通过 **引用计数** 和 **RAII（资源获取即初始化）** 机制，`shared_ptr` 和 `weak_ptr` 能够自动管理资源的分配和释放。

### 更深入的实现原理

- **本质是一个类**：`shared_ptr` 和 `weak_ptr` 本质上是一个模板类，内部通过一个控制块（control block）来管理资源和引用计数。
  - **控制块**：控制块中包含了资源的引用计数（`use_count`）和弱引用计数（`weak_count`），以及指向实际资源的裸指针。
  - **资源管理**：当 `use_count` 归零时，资源会被释放；当 `weak_count` 归零时，控制块本身会被释放。

- **构造智能指针实例**：当创建一个 `shared_ptr` 时，实际上是构造了一个智能指针类的实例，并将资源的裸指针和控制块绑定在一起。
  - `std::make_shared` 是推荐的方式，因为它会在堆上分配一个连续的内存块，同时存储控制块和资源，减少了内存分配的次数。

- **引用计数的更新**：
  - 每次拷贝或赋值一个 `shared_ptr`，`use_count` 会递增。
  - 每次销毁一个 `shared_ptr`，`use_count` 会递减。
  - `weak_ptr` 的 `weak_count` 仅在创建或销毁时更新，不影响 `use_count`。

- **线程安全性**：`shared_ptr` 的引用计数更新是线程安全的，但资源本身的访问需要额外的同步机制。

---

## 核心 API 和符号重载

### `shared_ptr` 的核心 API

- **构造函数**：
  - `std::shared_ptr<T> ptr(new T(...))`：直接管理动态分配的对象。
  - `std::make_shared<T>(...)`：推荐使用，避免手动调用 `new`。
- **成员函数**：
  - `use_count()`：返回当前引用计数。
  - `get()`：返回所管理的裸指针。
  - `reset()`：释放当前资源或替换为新的资源。
  - `swap()`：交换两个 `shared_ptr` 的资源。
- **运算符重载**：
  - 解引用运算符 `*` 和成员访问运算符 `->`，与裸指针行为一致。
  - 布尔转换运算符 `operator bool`，用于判断是否管理一个非空指针。

### `weak_ptr` 的核心 API

- **构造函数**：
  - 可以从一个 `shared_ptr` 构造 `weak_ptr`。
- **成员函数**：
  - `lock()`：返回一个指向资源的 `shared_ptr`，如果资源已被释放，则返回空的 `shared_ptr`。
  - `expired()`：判断资源是否已被释放。
  - `use_count()`：返回资源的引用计数。

---

## 使用场景

1. **`shared_ptr`**：
   - 适用于多个对象共享同一资源的场景，例如对象池、缓存等。
   - 自动管理资源生命周期，避免手动释放资源。

2. **`weak_ptr`**：
   - 用于解决循环引用问题。例如，在双向关联的对象中，使用 `weak_ptr` 打破循环引用。
   - 用于观察资源的生命周期，而不拥有资源。

---

## 示例代码

以下是一些常见的 `shared_ptr` 和 `weak_ptr` 用法示例：

### 示例 1：`shared_ptr` 的基本用法

```cpp
#include <iostream>
#include <memory>

int main() {
    // 使用 std::make_shared 创建 shared_ptr（推荐）
    auto ptr1 = std::make_shared<int>(10);
    std::cout << "Value: " << *ptr1 << ", Use count: " << ptr1.use_count() << std::endl;

    // 共享同一资源
    auto ptr2 = ptr1;
    std::cout << "After sharing, Use count: " << ptr1.use_count() << std::endl;

    // 离开作用域时，资源会被自动释放
    return 0;
}
```

### 示例 2：`weak_ptr` 解决循环引用问题

```cpp
#include <iostream>
#include <memory>

struct Node {
    std::shared_ptr<Node> next; // 指向下一个节点
    std::weak_ptr<Node> prev;   // 指向上一个节点（弱引用，避免循环引用）

    ~Node() {
        std::cout << "Node destroyed" << std::endl;
    }
};

int main() {
    auto node1 = std::make_shared<Node>();
    auto node2 = std::make_shared<Node>();

    node1->next = node2;
    node2->prev = node1; // 使用 weak_ptr 打破循环引用

    // 离开作用域时，资源会被正确释放
    return 0;
}
```

---

## 注意事项

1. **避免循环引用**：在 **双向关联** 的对象中，使用 `weak_ptr` 打破循环引用。
2. **性能开销**：`shared_ptr` 的引用计数操作需要一定的性能开销，尽量避免在性能敏感的场景中频繁使用。
3. **线程安全性**：
   - `shared_ptr` 的引用计数是线程安全的，但资源本身的访问需要额外的同步机制。
   - `weak_ptr` 的 `lock()` 操作不是线程安全的，需要注意并发访问问题。
4. **避免裸指针操作**：尽量通过智能指针的接口访问资源，减少错误风险。

> 线程安全版本： <https://en.cppreference.com/w/cpp/experimental/atomic_shared_ptr>

---

## 总结

`shared_ptr` 和 `weak_ptr` 是 C++ 标准库中提供的强大工具，用于安全地管理动态分配的资源。通过引用计数和 RAII 机制，它们能够有效避免内存泄漏和空悬指针问题。在实际使用中，合理选择智能指针类型，并注意避免循环引用和线程安全问题。
