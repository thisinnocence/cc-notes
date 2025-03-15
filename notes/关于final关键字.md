# 关于`final` 关键字

`final` 在 C++ 中用于限制某些类或成员函数的继承或重写行为。它主要有两种常见用途：

1. **防止类被继承**：通过在类声明时加上 `final`，可以防止该类被进一步继承。
2. **防止虚函数被重写**：通过在虚函数声明时加上 `final`，可以防止该函数在子类中被重写。

## **样例 1：防止类被继承**

当类被标记为 `final` 时，不能再有任何类从它派生。这样可以避免类被错误继承或避免不必要的扩展。

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    virtual void show() {
        cout << "Base class show() function" << endl;
    }
};

class Derived final : public Base {  // 不能再从 Derived 继承
public:
    void show() override {
        cout << "Derived class show() function" << endl;
    }
};

class AnotherDerived : public Derived {  // 编译错误：Derived 类是 final 的，不能继承
    void show() override {
        cout << "AnotherDerived class show() function" << endl;
    }
};

int main() {
    Derived d;
    d.show();  // 输出: Derived class show() function
    return 0;
}
```

## **样例 2：防止虚函数被重写**

通过在虚函数声明中加上 `final`，可以防止子类重写该虚函数。这样，如果子类尝试重写该函数，编译器会报错。

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    virtual void show() final {  // 该函数不能被重写
        cout << "Base class show() function" << endl;
    }
};

class Derived : public Base {
public:
    // 编译错误：尝试重写一个 final 的虚函数
    void show() override {
        cout << "Derived class show() function" << endl;
    }
};

int main() {
    Base* b = new Base();
    b->show();  // 输出: Base class show() function

    delete b;
    return 0;
}
```

## **总结：**

- **`final` 用于类**：防止类被继承，避免误用继承关系。
- **`final` 用于虚函数**：防止虚函数被进一步重写，确保函数的实现不会被修改。

`final` 是一种控制继承和多态行为的手段，可以提高代码的可维护性和安全性，避免不必要的类扩展和函数重写。
