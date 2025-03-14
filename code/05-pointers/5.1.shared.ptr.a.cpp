//
// 5.1.shared.ptr.cpp
// chapter 05 start pointers and memory management
// modern c++ tutorial
//
// created by changkun at changkun.de
// https://github.com/changkun/modern-cpp-tutorial
//

#include <iostream>
#include <memory>

void foo(std::shared_ptr<int> i)
{
    (*i)++;
}

void bar(std::shared_ptr<int> i)
{
    auto p = i;
    std::cout << "fun bar " << *p << std::endl;
    std::cout << "fun bar , use count " << p.use_count() << std::endl;
    /*
    disass /m 末尾反汇编指令，自动调用析构函数
    }
    0x000000000040134d <+173>:   lea    -0x10(%rbp),%rdi
    0x0000000000401351 <+177>:   callq  0x401ad0 <std::shared_ptr<int>::~shared_ptr()>
    0x0000000000401356 <+182>:   add    $0x50,%rsp
    0x000000000040135a <+186>:   pop    %rbp
    0x000000000040135b <+187>:   retq   
    0x000000000040135c <+188>:   mov    %rax,-0x18(%rbp)
    0x0000000000401360 <+192>:   mov    %edx,-0x1c(%rbp)
    0x0000000000401363 <+195>:   lea    -0x10(%rbp),%rdi
    0x0000000000401367 <+199>:   callq  0x401ad0 <std::shared_ptr<int>::~shared_ptr()>  // 析构
    0x000000000040136c <+204>:   mov    -0x18(%rbp),%rdi
    0x0000000000401370 <+208>:   callq  0x401100 <_Unwind_Resume@plt>
    0x0000000000401375:  nopw   %cs:0x0(%rax,%rax,1)
    0x000000000040137f:  nop

    析构函数，会减去引用，然后引用如果为0就是释放内存
    */
}

int main()
{
    // auto pointer = new int(10); // illegal, no direct assignment
    // std::shared_ptr construction
    // std::make_shared 就能够用来消除显式的使用 new, 使得写法对称
    // 当引用计数变为零的时候就会将对象自动删除, 调用delete
    auto pointer = std::make_shared<int>(10);
    auto pointer2 = pointer;    // reference count + 1
    auto pointer3 = pointer;    // reference count + 1
    
    
    foo(pointer);
    std::cout << *pointer << std::endl; // 11
    // get() 方法来获取原始指针
    int *p = pointer.get();             // does not increase reference count
    
    std::cout << "pointer.use_count() = " << pointer.use_count() << std::endl;   // 3
    std::cout << "pointer2.use_count() = " << pointer2.use_count() << std::endl; // 3
    std::cout << "pointer3.use_count() = " << pointer3.use_count() << std::endl; // 3

    // reset() 来减少一个引用计数， 并通过use_count()来查看一个对象的引用计数
    pointer2.reset();
    std::cout << "reset pointer2:" << std::endl;
    std::cout << "pointer.use_count() = " << pointer.use_count() << std::endl;   // 2
    std::cout << "pointer2.use_count() = " << pointer2.use_count() << std::endl; // 0
    std::cout << "pointer3.use_count() = " << pointer3.use_count() << std::endl; // 2
    
    pointer3.reset();
    std::cout << "reset pointer3:" << std::endl;
    std::cout << "pointer.use_count() = " << pointer.use_count() << std::endl;    // 1
    std::cout << "pointer2.use_count() = " << pointer2.use_count() << std::endl;  // 0
    std::cout << "pointer3.use_count() = " << pointer3.use_count() << std::endl;  // 0
    std::cout << *pointer << std::endl; // 11
    
    bar(pointer); // in bar, cout is: p.use_count is 3, 函数形参1个, 函数内局部变量1个, pointer自己
    std::cout << "pointer.use_count() = " << pointer.use_count() << std::endl; // 1


    // Before leaving the scope, the pointer is destructed and
    // the reference count is reduced to 0
    return 0;
}
