// 在C++中，当对象的析构函数被调用时（例如通过delete或离开作用域），
// 其所有成员变量的析构函数会自动按声明顺序的逆序依次调用。

#include <cstdio>

class Inner {
public:
    int a;
    Inner(int a):a(a){};
    ~Inner() {
        printf("Inner(%d) destructor called\n", a);
    }
};

class Outer {
public:
    Inner inner1;
    Inner inner2;
    Outer() : inner1(1), inner2(2) {}  // 使用构造函数初始化列表
    ~Outer() {
        printf("Outer destructor called\n");
    }
};

int main() {
    Outer* outer = new Outer();
    delete outer;  // 触发Outer和Inner的析构
    return 0;
}

/* @output: 
Outer destructor called
Inner(2) destructor called
Inner(1) destructor called
*/
