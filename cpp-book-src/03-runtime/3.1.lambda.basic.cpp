//
// 3.1.lambda.basic.cpp
// chapter 03 runtime enhancement
// modern c++ tutorial
//
// created by changkun at changkun.de
// https://github.com/changkun/modern-cpp-tutorial
//


#include <iostream>
#include <memory>  // std::make_unique
#include <utility> // std::move

void lambda_value_capture() {
    int value = 1;
    // 值捕获的前提是变量可以拷贝
    // 被捕获的变量在 Lambda 表达式被创建时拷贝， 而非调用时才拷贝
    auto copy_value = [value] { // 表达式被创建时，value = 1
        return value;
    };
    value = 100;
    auto stored_value = copy_value();
    std::cout << "stored_value = " << stored_value << std::endl;
    // At this moment, stored_value == 1, and value == 100.
    // Because copy_value has copied when its was created.
}

void lambda_reference_capture() {
    int value = 1;
    // 引用捕获保存的是引用
    auto copy_value = [&value] {
        return value;
    };
    value = 100;
    auto stored_value = copy_value();
    std::cout << "stored_value = " << stored_value << std::endl;
    // At this moment, stored_value == 100, value == 100.
    // Because copy_value stores reference
}

// 捕获列表可以起到传递外部数据的作用
// 值捕获、引用捕获都是已经在外层作用域声明的变量
// 因此这些捕获方式捕获的均为左值，而不能捕获右值
// C++14 给与了我们方便，允许捕获的成员用任意的表达式进行初始化, 允许右值

void lambda_expression_capture() {
    auto important = std::make_unique<int>(1);

    // important是unique_ptr 独占指针，是不能够被 "=" 值捕获到
    // std::move 将其转移为右值，在表达式中初始化
    auto add = [v1 = 1, v2 = std::move(important)](int x, int y) -> int {
        return x+y+v1+(*v2);
    };
    std::cout << add(3,4) << std::endl; // 9
}

void lambda_generic() {
    // C++14 开始，Lambda 函数的形式参数可以使用 auto 关键字来产生意义上的泛型
    auto generic = [](auto x, auto y) {
        return x+y;
    };
    
    std::cout << generic(1, 2) << std::endl;      // 3
    std::cout << generic(1.1, 2.2) << std::endl;  // 3.3
}

int main() {
    lambda_value_capture();
    lambda_reference_capture();
    lambda_expression_capture();
    lambda_generic();
    return 0;
}
