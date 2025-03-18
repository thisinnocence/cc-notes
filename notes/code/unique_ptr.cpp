#include <iostream>
#include <utility> // 引入 std::forward 和 std::move

template <typename T>
class unique_ptr {
private:
    T* ptr; // 管理的原始指针

public:
    // 构造函数，接受一个原始指针，默认为 nullptr
    explicit unique_ptr(T* p = nullptr) : ptr(p) {}

    // 析构函数，释放资源
    ~unique_ptr() {
        delete ptr;
    }

    // 禁用拷贝构造函数和拷贝赋值运算符，确保唯一所有权
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    // 移动构造函数，转移所有权
    unique_ptr(unique_ptr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr; // 将源指针置空
    }

    // 移动赋值运算符，转移所有权并释放当前资源
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) { // 防止自我赋值
            delete ptr;       // 释放当前资源
            ptr = other.ptr;  // 转移所有权
            other.ptr = nullptr; // 将源指针置空
        }
        return *this;
    }

    // 解引用运算符，返回引用
    T& operator*() const {
        return *ptr;
    }

    // 箭头运算符，返回原始指针
    T* operator->() const {
        return ptr;
    }

    // 获取原始指针
    T* get() const {
        return ptr;
    }

    // 释放所有权，返回原始指针并将内部指针置空
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    // 重置指针，释放当前资源并管理新的指针
    void reset(T* p = nullptr) {
        if (ptr != p) { // 防止重复释放
            delete ptr;
            ptr = p;
        }
    }
};

// 工厂函数，用于创建 unique_ptr
// 使用完美转发将参数传递给对象的构造函数
template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

struct Test {
    void say_hello() const {
        std::cout << "Hello" << std::endl;
    }
};

int main() {
    // 使用 make_unique 创建 unique_ptr
    auto uptr = make_unique<Test>();
    uptr->say_hello(); // 输出: Hello

    // 转移所有权
    unique_ptr<Test> uptr2 = std::move(uptr);
    if (!uptr.get()) { // 检查 uptr 是否为空
        std::cout << "uptr is now empty." << std::endl; // 输出: uptr is now empty.
    }

    uptr2->say_hello(); // 输出: Hello
    return 0;
}
