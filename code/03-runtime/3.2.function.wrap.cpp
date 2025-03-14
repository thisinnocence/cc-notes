//
// 3.2.function.wrap.cpp
// chapter 03 runtime enhancement
// modern c++ tutorial
//
// created by changkun at changkun.de
// https://github.com/changkun/modern-cpp-tutorial
//

#include <functional>
#include <iostream>
#include <cstdio>

using foo = void(int);  // function pointer
void functional(foo f) {
    f(1);
}

int foo2(int para) {
    return para;
}

int foo3(int a, int b, int c) {
    printf("foo3: a %d, b %d, c %d\n", a, b, c);
    return a + b + c;
}

int main() {

    auto f = [](int value) {
        std::cout << value << std::endl;
    };
    functional(f);  // call by function pointer,  1
    f(1);           // call by lambda expression, 1

    // std::function wraps a function that take int paremeter and returns int value
    std::function<int(int)> func = foo2;

    int important = 10;
    std::function<int(int)> func2 = [&](int value) -> int {
        return 1+value+important;
    };
    std::cout << func(10) << std::endl;  // 10
    std::cout << func2(10) << std::endl; // 21

    // bind parameter 1, 2 on function foo, and use std::placeholders::_1 as placeholder
    // for the first parameter.
    // 因为已经知道了 b, c应该传什么，第1个参数还不知道
    auto bindFoo = std::bind(foo3, std::placeholders::_1, 1,2);

    // 感觉这种写法在如下场景很有用：
    // 多次调用，但是某个参数是固定的，只有剩余几个不固定
    // 直接bind一下，后面调用就方便很多

    // when call bindFoo, we only need one param left, 就是参数a
    std::cout << "bindFoo(3) is: " << bindFoo(3) << std::endl;
    // printf: foo3: a 3, b 1, c 2
    // 6

    return 0;
}
