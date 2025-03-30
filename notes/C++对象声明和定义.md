# C++ 对象声明和定义

C++ 中对象的声明和定义有多种方式，具体取决于构造函数的类型、赋值方式以及参数传递方式等。以下是详细的解释和示例：

## 直接调用构造函数

直接调用构造函数是最常见的对象定义方式：

```cpp
class MyClass {
public:
    MyClass(int x) : value(x) {}
private:
    int value;
};

MyClass obj(10); // 直接调用构造函数
```

当构造函数有多个参数时，显式传递参数：

```cpp
class MyClass {
public:
    MyClass(int x, int y) : value1(x), value2(y) {}
private:
    int value1, value2;
};

MyClass obj(10, 20); // 调用带两个参数的构造函数
```

如果构造函数有多个入参，但是除了 1 个没有默认参数外，其他都有默认参数，也可以通过隐式转换调用：

```cpp
class MyClass {
public:
    // 这种隐式转换可能导致意外的行为，最佳实践是使用 explicit 关键字来防止隐式转换
    MyClass(int x, int y = 0) : value1(x), value2(y) {}
private:
    int value1, value2;
};

MyClass obj = 100; // 隐式调用 MyClass(int, int) 构造函数，等价于 MyClass(100, 0)
```

> **注意**：从 C++11 开始，建议使用 `explicit` 关键字来避免隐式转换带来的潜在问题。

## 使用赋值初始化

通过赋值初始化对象：

```cpp
MyClass obj = MyClass(20); // 等价于直接调用构造函数
```

## 使用列表初始化

C++11 引入了列表初始化，可以使用 `{}` 进行初始化：

```cpp
MyClass obj{30}; // 列表初始化
```

聚合初始化：

```cpp
struct Point {
    int x;
    int y;
};

Point p{1, 2}; // 聚合初始化
```

---

## 特殊构造函数定义对象

### 默认构造函数

如果类没有定义任何构造函数，编译器会生成一个隐式默认构造函数：

```cpp
class MyClass {};

MyClass obj; // 使用隐式默认构造函数
```

如果类中定义了其他构造函数，但没有显式定义默认构造函数，则默认构造函数不会被自动生成：

```cpp
class MyClass {
public:
    MyClass(int x) : value(x) {}
private:
    int value;
};

// MyClass obj; // 编译错误，因为没有默认构造函数
```

可以显式声明默认构造函数：

```cpp
class MyClass {
public:
    MyClass() = default; // 显式声明默认构造函数
    MyClass(int x) : value(x) {}
private:
    int value = 0; // 默认值
};

MyClass obj; // 使用显式声明的默认构造函数
```

### 拷贝构造函数

拷贝构造函数用于对象的拷贝初始化：

```cpp
class MyClass {
public:
    MyClass() = default; // 显式声明使用编译器生成的默认构造函数
    MyClass(const MyClass&) = default; // 显式声明使用编译器生成的拷贝构造函数
    MyClass& operator=(const MyClass&) = default; // 显式声明使用编译器生成的拷贝赋值运算符
    ~MyClass() = default; // 显式声明使用编译器生成的析构函数
private:
    int value = 0;
};

MyClass obj1(80);
MyClass obj2 = obj1; // 调用拷贝构造函数
```

根据 C++ 的三/五法则，如果你定义了以下任意一个函数，通常需要定义所有这些函数：

- 析构函数
- 拷贝构造函数
- 拷贝赋值运算符
- （C++11）移动构造函数
- （C++11）移动赋值运算符

### 移动构造函数

C++11 引入了移动构造函数，用于高效地转移资源：

```cpp
class MyClass {
public:
    MyClass(int x) : value(x) {} 
    MyClass(MyClass&& other) noexcept : value(other.value) {
        other.value = 0; // 置空源对象的资源
    }
    // 移动赋值运算符
    MyClass& operator=(MyClass&& other) noexcept {
        if (this != &other) {
            value = other.value;
            other.value = 0;
        }
        return *this;
    }
private:
    int value;
};

MyClass obj1(90);
MyClass obj2 = std::move(obj1); // 调用移动构造函数
```

---

## 动态分配对象

### 使用 `new` 关键字

`new` 关键字用于动态分配内存并调用构造函数：

```cpp
class MyClass {
public:
    MyClass(int x) : value(x) {}
private:
    int value;
};

MyClass* obj = new MyClass(10); // 动态分配并调用构造函数
delete obj; // 手动释放内存
```

### 使用数组形式的动态分配

`new` 关键字还可以用于动态分配对象数组：

```cpp
MyClass* objArray = new MyClass[5]; // 调用默认构造函数创建数组
delete[] objArray; // 必须使用 delete[] 来删除数组
```

注意：数组形式的动态分配要求类必须有默认构造函数。如果类没有默认构造函数，则需要显式初始化每个元素：

```cpp
MyClass* objArray = new MyClass[3]{MyClass(1), MyClass(2), MyClass(3)};
```

### 使用智能指针管理动态对象

从 C++11 开始，推荐使用智能指针管理动态分配的对象，以避免内存泄漏，比如 `std::unique_ptr` 和 `std::shared_ptr`：

```cpp
#include <memory>

std::unique_ptr<MyClass> obj = std::make_unique<MyClass>(50);
std::shared_ptr<MyClass> shared_obj = std::make_shared<MyClass>(50);
```

智能指针会自动管理内存，无需手动调用 `delete`。

---

## 总结

C++ 提供了多种方式声明和定义对象，包括直接调用构造函数、列表初始化、动态分配等。根据实际需求选择合适的方式，同时注意避免隐式转换带来的潜在问题。
