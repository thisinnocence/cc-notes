#include <iostream>
#include <memory>

using namespace std;

class Animal {
public:
    Animal() { cout << "Construct Animal\n"; }
    // 父类如果被继承，析构函数一定要virtual，好让编译器delete父类指针时，去找虚函数表
    virtual ~Animal() { cout << "destruct Animal\n"; }
    virtual void eat() {
        cout << "Animal eat\n";
    }
    void show() {
        cout << "Animal show\n";
    }
};

// 继承如果不指定访问权限，默认是 private
// 子类构造时，先构造父类，再构造子类
// 子类析构时，先析构子类，再析构父类
class Dog: public Animal {
public:
    Dog() { cout << "Construct Dog\n"; }
    ~Dog() { cout << "destruct Dog\n"; }
    void eat() override {
        cout << "Dog eat\n";
    }
};

class Cat: public Animal {
public:
    Cat() { cout << "Construct Cat\n"; }
    ~Cat() { cout << "destruct Cat\n"; }
    void eat() override {
        cout << "Cat eat\n";
    }
};

int main()
{
    unique_ptr<Animal> ani = make_unique<Cat>();
    ani->eat();
    ani->show();

    cout << "----\n";
    // reset 后，会自动析构原来的 raw-data, 不是推荐写法，直接make赋值，会自动释放老的资源的
    // ani.reset(new Dog);
    ani = make_unique<Dog>(); 
    cout << "----\n";

    ani->eat();
    ani->show();
    return 0;
}