# share pointer

## chat-gpt解释的很到位

Q:shared pointer 源代码

A: 

```cpp
#include <iostream>

template<typename T>
class SharedPtr {
private:
    T* ptr;          // 指向所管理对象的原始指针
    int* refCount;   // 引用计数

public:
    // 构造函数，接受一个指向动态分配对象的原始指针
    explicit SharedPtr(T* p = nullptr) : ptr(p), refCount(new int(1)) {}

    // 拷贝构造函数，增加引用计数
    SharedPtr(const SharedPtr& other) : ptr(other.ptr), refCount(other.refCount) {
        (*refCount)++;
    }

    // 析构函数，减少引用计数并释放资源
    ~SharedPtr() {
        (*refCount)--;
        if (*refCount == 0) {
            delete ptr;
            delete refCount;
        }
    }

    // 重载赋值运算符，确保正确地增加和减少引用计数
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            // 先释放当前资源
            (*refCount)--;
            if (*refCount == 0) {
                delete ptr;
                delete refCount;
            }
            // 更新指针和引用计数
            ptr = other.ptr;
            refCount = other.refCount;
            (*refCount)++;
        }
        return *this;
    }

    // 获取指向所管理对象的原始指针
    T* get() const {
        return ptr;
    }

    // 获取引用计数
    int use_count() const {
        return *refCount;
    }
};

int main() {
    // 使用共享指针
    SharedPtr<int> p1(new int(42));
    std::cout << "use count: " << p1.use_count() << std::endl;

    {
        SharedPtr<int> p2 = p1; // 使用拷贝构造函数
        std::cout << "use count: " << p2.use_count() << std::endl;
    }

    std::cout << "use count: " << p1.use_count() << std::endl;

    return 0;
}
```

Q: 这个近似的源代码，为什么用     `int* refCount;`   // 引用计数
   而不是 直接用int，不是指针呢 ?

A: 这个实现选择使用 `int*` 类型的指针来保存引用计数，而不是直接使用 int 类型，
是因为这样可以确保多个 SharedPtr 对象共享同一个引用计数。
考虑到 SharedPtr 对象可能会被拷贝或赋值给其他对象，如果使用 int 类型来保存引用计数，
每次拷贝 SharedPtr 对象时，会复制一份新的引用计数，这样就无法确保所有 SharedPtr 对象共享同一个引用计数，
从而导致引用计数的管理出现问题，可能会导致内存泄漏或者错误释放。
而使用 `int*` 类型的指针，可以确保多个 SharedPtr 对象共享同一个引用计数。当一个 SharedPtr 对象被拷贝或赋值给另一个对象时，
它们将共享同一个引用计数，这样就可以确保引用计数的增加和减少是同步的，从而保证了引用计数的正确管理。

