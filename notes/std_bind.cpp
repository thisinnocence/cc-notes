/**
 * @brief std::bind的用法示例
 * 
 * @details
 * std::bind是一个函数适配器，用于将可调用对象与参数绑定，生成一个新的可调用对象。
 * 它可以：
 * - 固定一个可调用对象的某些参数
 * - 改变参数的调用顺序
 * - 将成员函数转换为普通函数对象
 * 
 * @usage 基本语法
 * auto new_func = std::bind(callable, args...);
 * 
 * @note 参数绑定说明
 * - std::placeholders::_1, _2, ... 表示新函数的第1,2,...个参数
 * - 直接传值的参数将被固定为指定值
 * - 成员函数绑定格式：std::bind(&Class::method, instance_ptr, args...)
 * 
 * @example
 * auto add_five = std::bind(add, 5, std::placeholders::_1);
 * // add_five(10) 等价于 add(5, 10)
 */

#include <functional>
#include <iostream>

// 普通函数
int add(int a, int b)
{
    return a + b;
}

// 测试类
class Calculator {
public:
    int multiply(int x, int y)
    {
        return x * y;
    }
};

int main()
{
    // 定义std::funtion数组，用于存储std::bind生成的可调用对象
    //     区别:函数指针只能指向签名完全匹配的函数, 并且不能存储有状态的可调用对象
    std::function<int(int)> func_array[2];

    // 1. 绑定普通函数 add 并存储
    func_array[0] = std::bind(add, std::placeholders::_1, 20);

    // 2. 绑定类成员函数并存储
    Calculator calc;
    func_array[1] = std::bind(&Calculator::multiply, &calc, std::placeholders::_1, 2);

    // 通过数组调用函数
    std::cout << "func_array[0](5) = " << func_array[0](5) << std::endl;  // 输出25
    std::cout << "func_array[1](4) = " << func_array[1](4) << std::endl;  // 输出8

    return 0;
}