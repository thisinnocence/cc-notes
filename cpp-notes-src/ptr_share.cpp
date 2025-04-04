#include <iostream>
#include <utility> // for std::forward

template<typename T>
class SharedPtr {
private:
    T* ptr;
    int* refCount; // 这里用指针，因为多个实例都会用到，必须是指针的赋值，但指向同一个data

public:
    // 避免隐式转换，必须明确使用构造函数，而不是不同类型的赋值那种隐式转换
    explicit SharedPtr(T* p = nullptr) : ptr(p), refCount(p ? new int : nullptr) {}

    SharedPtr(const SharedPtr& other) : ptr(other.ptr), refCount(other.refCount) {
        if (refCount) (*refCount)++;
    }

    ~SharedPtr() {
        std::cout << "[info] ~SharedPtr() called!\n";
        release();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount;
            if (refCount) (*refCount)++;
        }
        return *this;
    }

    T* get() const { return ptr; }
    int use_count() const { return refCount ? *refCount : 0; }

private:
    void release() {
        if (refCount) {
            (*refCount)--;
            if (*refCount == 0) {
                std::cout << "[info] count is 0, so delete origin data pointer!\n";
                delete ptr;
                delete refCount;
                ptr = nullptr;
                refCount = nullptr;
            }
        }
    }
};

template<typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

void test()
{
    SharedPtr<int> p1 = make_shared<int>(42);
    std::cout << "use count: " << p1.use_count() << std::endl; // 1

    {   // 开启一个新作用域，离开后p2没有人用，会触发RAII自动delete析构
        SharedPtr<int> p2 = p1;
        std::cout << "use count: " << p2.use_count() << std::endl; // 2
        std::cout << "p2 levave {}, will auto-delete!\n";
    }
    std::cout << "use count: " << p1.use_count() << std::endl; // 1
    std::cout << "p1 levave test, will auto-delete!\n";
}

int main() {
    test();
    return 0;
}
