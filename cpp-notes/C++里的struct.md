# C++里的`struct`

`struct`是C++中一个重要的复合数据类型，它继承自C语言，但在C++中有了更多的功能和灵活性。本文将详细探讨C++中`struct`的特点，并与C语言中的`struct`进行对比。

---

## 1. C++中的`struct`与C的区别

### 1.1 访问控制

- 在C语言中，`struct`中的所有成员默认都是**公共的**。
- 在C++中，`struct`中的成员默认也是**公共的**，但C++允许为`struct`添加访问控制修饰符（如`private`和`protected`），这使得`struct`更接近于`class`。

```cpp
struct Example {
    int publicMember;   // 默认是public
private:
    int privateMember;  // 需要显式声明为private
};
```

### 1.2 支持成员函数

- C语言中的`struct`只能包含数据成员。
- C++中的`struct`可以包含成员函数、构造函数、析构函数、静态成员等。

```cpp
struct Example {
    int x;
    Example(int val) : x(val) {}  // 构造函数
    void display() {             // 成员函数
        std::cout << "x = " << x << std::endl;
    }
};
```

### 1.3 命名空间支持

- C++中的`struct`可以定义在命名空间中，而C语言没有命名空间的概念。

```cpp
namespace MyNamespace {
    struct Example {
        int x;
    };
}
```

---

## 2. 声明与定义的形式

在C语言中，通常需要使用`typedef`来简化`struct`的使用：

```c
typedef struct {
    int x;
    int y;
} Point;
```

在C++中，不需要`typedef`，可以直接使用`struct`的名字：

```cpp
struct Point {
    int x;
    int y;
};
Point p;  // 直接使用
```

---

## 3. `struct`与`class`的区别

在C++中，`struct`和`class`的主要区别在于**默认的访问控制**：

- `struct`的成员默认是`public`。
- `class`的成员默认是`private`。

除此之外，`struct`和`class`在功能上几乎是等价的。

```cpp
struct StructExample {
    int x;  // 默认public
};

class ClassExample {
    int x;  // 默认private
};
```

---

## 4. 内存布局与结构对齐

### 4.1 内存布局

- `struct`的内存布局是按照成员声明的顺序进行的，但可能会因为对齐要求而插入填充字节（padding）。
- C++中的`struct`与C语言中的`struct`在内存布局上通常是兼容的。

### 4.2 结构对齐

- 结构对齐是为了提高内存访问效率，编译器会按照对齐规则调整成员的存储位置。
- 可以使用`#pragma pack`或`alignas`关键字来控制对齐方式。

```cpp
#pragma pack(1)  // 强制1字节对齐
struct PackedStruct {
    char a;
    int b;
};
#pragma pack()  // 恢复默认对齐
```

或者使用C++11的`alignas`：

```cpp
struct AlignedStruct {
    char a;
    alignas(8) int b;  // 强制8字节对齐
};
```

---

## 5. 使用场景

- `struct`通常用于表示简单的数据结构，例如点、矩形等。
- 如果需要封装复杂的逻辑，建议使用`class`。

---

## 6. 总结

C++中的`struct`在功能上比C语言中的`struct`更强大，几乎可以看作是`class`的一个变体。选择`struct`还是`class`，更多是基于代码风格和语义上的考虑：

- 如果主要是数据结构，推荐使用`struct`。
- 如果需要封装复杂逻辑，推荐使用`class`。

```cpp
// 示例：使用struct表示简单数据结构
struct Point {
    int x, y;
};

// 示例：使用class封装复杂逻辑
class Rectangle {
    int width, height;
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    int area() { return width * height; }
};
```

希望本文能帮助你更好地理解C++中的`struct`及其应用场景！
