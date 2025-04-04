//
// 3.3.rvalue.cpp
// modern c++ tutorial
//
// created by changkun at changkun.de
// https://github.com/changkun/modern-cpp-tutorial
//


#include <iostream>
#include <string>

void reference(std::string& str) {
    std::cout << "lvalue" << std::endl;
}
void reference(std::string&& str) {
    std::cout << "rvalue" << std::endl;
}

// **左值** (lvalue, left value)，顾名思义就是赋值符号左边的值。准确来说，
//   左值是表达式（不一定是赋值表达式）后依然存在的持久对象。
// **右值** (rvalue, right value)，右边的值，是指表达式结束后就不再存在的临时对象。
//   C++11 将右值的概念进行了进一步的划分，分为：纯右值、将亡值。
//    纯右值(prvalue, pure rvalue)，纯粹的右值：
//        1 纯粹的字面量，例如 `10`, `true`；
//        2 求值结果相当于字面量或匿名临时对象，例如 `1+2`。
//        3 非引用返回的临时变量、运算表达式产生的临时变量
//        原始字面量、Lambda 表达式都属于纯右值。
//    将亡值：要拿到一个将亡值，就需要用到右值引用：`T &&`，其中 `T` 是类型。
//      右值引用的声明让这个临时值的生命周期得以延长、只要变量还活着，那么将亡值将继续存活。
int main()
{
    std::string  lv1 = "string,";       // lv1 is a lvalue
    // std::string&& r1 = lv1;           // illegal, rvalue can't ref to lvalue
    std::string&& rv1 = std::move(lv1); // legal, std::move can convert lvalue to rvalue
    std::cout << rv1 << std::endl;      // string,
    
    const std::string& lv2 = lv1 + lv1; // legal, const lvalue reference can extend temp variable's lifecycle
    // lv2 += "Test";                   // illegal, const ref can't be modified
    std::cout << lv2 << std::endl;      // string,string
    
    std::string&& rv2 = lv1 + lv2;      // legal, rvalue ref extend lifecycle
    rv2 += "string";                    // legal, non-const reference can be modified
    std::cout << rv2 << std::endl;      // string,string,string,
    
    reference(rv2);                     // output: lvalue
    
    return 0;
}
