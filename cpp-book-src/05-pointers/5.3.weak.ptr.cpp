//
// 5.3.weak.ptr.cpp
// chapter 05 start pointers and memory management
// modern c++ tutorial
//
// created by changkun at changkun.de
// https://github.com/changkun/modern-cpp-tutorial
//

#include <iostream>
#include <memory>

struct A;
struct B;

struct A {
    std::weak_ptr<B> pointer; // 不会增加对象的引用计数, 如果是shared_prt，复制会增加引用计数
    ~A() {
        std::cout << "A 被销毁" << std::endl;
    }
};

struct B {
    std::weak_ptr<A> pointer;
    ~B() {
        std::cout << "B 被销毁" << std::endl;
    }
};

int main() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();
    a->pointer = b;
    b->pointer = a;
    std::cout << a.use_count() << std::endl; // 1
    std::cout << b.use_count() << std::endl; // 1

	// 打印是：
	//   B 被销毁
	//   A 被销毁
    return 0;
}

