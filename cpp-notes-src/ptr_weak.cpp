#include <cstdio>
#include <string>
#include <atomic>

// 简单的引用计数器
class RefCounter {
public:
    void addRef() { ++strongRefs; }
    void releaseRef() { --strongRefs; }
    void addWeakRef() { ++weakRefs; }
    void releaseWeakRef() { --weakRefs; }
    int strongCount() const { return strongRefs; }
    int weakCount() const { return weakRefs; }

private:
    std::atomic<int> strongRefs{0};
    std::atomic<int> weakRefs{0};
};

// 自定义的智能指针类
template <typename T>
class SharedPtr {
public:
    // 修改默认构造函数，避免创建不必要的 RefCounter
    SharedPtr(T* ptr = nullptr) : ptr(ptr), refCounter(ptr ? new RefCounter() : nullptr) {
        if (ptr) refCounter->addRef();
    }

    // 增加一个构造函数，用于 WeakPtr::lock() 调用
    SharedPtr(T* ptr, RefCounter* counter) : ptr(ptr), refCounter(counter) {
        if (ptr)
            refCounter->addRef();
    }

    ~SharedPtr() {
        if (ptr) {
            refCounter->releaseRef();
            if (refCounter->strongCount() == 0) {
                delete ptr;
                if (refCounter->weakCount() == 0) {
                    delete refCounter;
                }
            }
        }
    }

    SharedPtr(const SharedPtr& other) : ptr(other.ptr), refCounter(other.refCounter) {
        if (ptr)
            refCounter->addRef();
    }

    // 修改赋值操作符，避免直接调用析构函数
    //     赋值之后，原指针指向了新的对象，所以老的数据ptr引用要减去
    //     同时，新的对象又被1个指针指向了，所以引用要增加
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            // 先保存旧的数据
            T* oldPtr = ptr;
            RefCounter* oldCounter = refCounter;

            // 更新新的数据
            ptr = other.ptr;
            refCounter = other.refCounter;
            if (ptr) refCounter->addRef();

            // 释放旧的数据
            if (oldPtr) {
                oldCounter->releaseRef();
                if (oldCounter->strongCount() == 0) {
                    delete oldPtr;
                    if (oldCounter->weakCount() == 0) {
                        delete oldCounter;
                    }
                }
            }
        }
        return *this;
    }

    // 添加移动构造函数，移动语义，直接把metadata赋值过去即可，首次构造所以直接赋值
    SharedPtr(SharedPtr&& other) noexcept
        : ptr(other.ptr), refCounter(other.refCounter) {
        other.ptr = nullptr;
        other.refCounter = nullptr;
    }

    // 添加移动赋值操作符，移动语义，老的直接接管赋值的对象的所有权，同时老的对象ref要减
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            // 释放当前资源
            if (ptr) {
                refCounter->releaseRef();
                if (refCounter->strongCount() == 0) {
                    delete ptr;
                    if (refCounter->weakCount() == 0) {
                        delete refCounter;
                    }
                }
            }

            // 转移所有权
            ptr = other.ptr;
            refCounter = other.refCounter;
            other.ptr = nullptr;
            other.refCounter = nullptr;
        }
        return *this;
    }

    T* get() const { return ptr; }
    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }

    // 添加 bool 转换操作符，用于判断指针是否有效
    // 允许你直接将 SharedPtr 用作布尔表达式，而不需要显式调用 get() 或检查 ptr 是否为 nullptr。
    operator bool() const { return ptr != nullptr; }

    RefCounter* getRefCounter() const { return refCounter; }

private:
    T* ptr;
    RefCounter* refCounter;
};

// 自定义的弱指针类
template <typename T> class WeakPtr {
public:
    WeakPtr() : ptr(nullptr), refCounter(nullptr) {}

    WeakPtr(const SharedPtr<T>& sharedPtr) : ptr(sharedPtr.get()), refCounter(sharedPtr.getRefCounter()) {
        if (refCounter)
            refCounter->addWeakRef();
    }

    ~WeakPtr() {
        if (refCounter) {
            refCounter->releaseWeakRef();
            if (refCounter->strongCount() == 0 && refCounter->weakCount() == 0) {
                delete refCounter;
            }
        }
    }

    WeakPtr(const WeakPtr& other) : ptr(other.ptr), refCounter(other.refCounter) {
        if (refCounter)
            refCounter->addWeakRef();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            // 先保存旧的计数器
            RefCounter* oldCounter = refCounter;

            // 更新指针和计数器
            ptr = other.ptr;
            refCounter = other.refCounter;

            // 增加新的弱引用计数
            if (refCounter)
                refCounter->addWeakRef();

            // 释放旧的弱引用计数
            if (oldCounter) {
                oldCounter->releaseWeakRef();
                if (oldCounter->strongCount() == 0 && oldCounter->weakCount() == 0) {
                    delete oldCounter;
                }
            }
        }
        return *this;
    }

    // 添加移动构造函数
    WeakPtr(WeakPtr&& other) noexcept
        : ptr(other.ptr), refCounter(other.refCounter) {
        other.ptr = nullptr;
        other.refCounter = nullptr;
    }

    // 添加移动赋值操作符
    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            if (refCounter) refCounter->releaseWeakRef();
            ptr = other.ptr;
            refCounter = other.refCounter;
            other.ptr = nullptr;
            other.refCounter = nullptr;
        }
        return *this;
    }

    SharedPtr<T> lock() const {
        // 检查指针和引用计数是否有效
        if (!ptr || !refCounter || refCounter->strongCount() <= 0) {
            return SharedPtr<T>();
        }
        return SharedPtr<T>(ptr, refCounter);
    }

private:
    T* ptr;
    RefCounter* refCounter;
};

// 在 SharedPtr 类的定义后，添加模板工厂函数
template<typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

// 添加 WeakPtr 的工厂函数
template<typename T>
WeakPtr<T> make_weak(const SharedPtr<T>& shared) {
    return WeakPtr<T>(shared);
}

// 示例类 Parent 和 Child
class Child; // 前向声明

class Parent {
public:
    std::string name;
    SharedPtr<Child> child; // 自定义的SharedPtr

    Parent(const std::string& name) : name(name) { printf("Parent %s created.\n", name.c_str()); }

    ~Parent() { printf("Parent %s destroyed.\n", name.c_str()); }
};

class Child {
public:
    std::string name;
    WeakPtr<Parent> parent; // 自定义的WeakPtr

    Child(const std::string& name) : name(name) { printf("Child %s created.\n", name.c_str()); }

    ~Child() { printf("Child %s destroyed.\n", name.c_str()); }
};

int main() {
    {
        // 使用工厂函数创建对象
        auto parent = make_shared<Parent>("Parent1");
        auto child = make_shared<Child>("Child1");

        parent->child = child;  // Parent拥有Child的共享所有权
        // 使用 make_weak 创建弱引用
        child->parent = make_weak(parent);

        // 输出Parent和Child的关系
        printf("%s has a child named %s.\n", parent->name.c_str(), parent->child->name.c_str());
        if (auto parentPtr = child->parent.lock()) { // 检查WeakPtr是否有效
            printf("%s's parent is %s.\n", child->name.c_str(), parentPtr->name.c_str());
        }
    } // 代码块结束时，Parent和Child对象被正确销毁，无内存泄漏
    return 0;
}
