/**
 * @brief 类内成员函数的隐式内联(Implicit Inline)示例
 *
 * @details
 * 根据C++标准(C++11 § 9.3/2)，类定义内部的成员函数定义会被隐式地声明为inline。
 * 
 * @note 重要说明
 * 1. 隐式inline特性：
 *    - 在类定义内部直接定义的成员函数自动成为inline候选
 *    - inline只是对编译器的建议，不是强制性的
 *    - 编译器可能会忽略这个建议，特别是对于过大或复杂的函数
 * 
 * 2. ODR (One Definition Rule) 规则对类内成员函数的应用：
 *    - 类定义中的内联成员函数可以在多个.cpp文件中出现
 *    - 这是因为它们是隐式inline的，不违反ODR规则
 *    - 这与普通inline函数不同，是类定义的特殊性质
 *    - 在不同编译单元中的定义必须完全相同
 * 
 * @warning 潜在问题
 * - 类内定义的实现会被包含在头文件中
 * - 当头文件被多个源文件包含时，会增加编译时间
 * - 可能导致代码膨胀，影响指令缓存效率
 * 
 * @best_practice
 * - 简单、短小的函数适合类内定义
 * - 复杂的实现建议放在源文件中定义
 */

#include <iostream>

class Example {
public:
    // 方法1：在类内定义，隐式inline
    void methodA()
    {
        std::cout << "Method A" << std::endl;
    }

    // 方法2：只在类内声明
    void methodB();
};

// 类外定义，非inline
void Example::methodB()
{
    std::cout << "Method B" << std::endl;
}

int main()
{
    Example e;
    e.methodA();
    e.methodB();
    return 0;
}