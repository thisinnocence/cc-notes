#include <functional>
#include <iostream>
#include <memory>

using namespace std;

class Animal {
public:
    Animal() { cout << "Construct Animal\n"; }
    // 父类如果被继承，析构函数一定要virtual，好让编译器delete父类指针时，去找虚函数表
    virtual ~Animal() { cout << "Destruct Animal\n"; }
    virtual void Eat() { cout << "Animal eat\n"; }
    void Show() { cout << "Animal show\n"; }
};

// 继承如果不指定访问权限，默认是 private
// 子类构造时，先构造父类，再构造子类
// 子类析构时，先析构子类，再析构父类
// 子类不想在被后续继承的时候，用 final 关键字
class Dog final : public Animal {
public:
    Dog() { cout << "Construct Dog\n"; }
    ~Dog() { cout << "Destruct Dog\n"; }
    void Eat() override { cout << "Dog eat\n"; }
};

class Cat : public Animal {
public:
    Cat() { cout << "Construct Cat\n"; }
    ~Cat() { cout << "Destruct Cat\n"; }
    void Eat() override { cout << "Cat eat\n"; }
};

std::function<int(int, int)> add = [](int a, int b) { return a + b; };

auto sub = [](int a, int b) { return a - b; };

int main() {
    unique_ptr<Animal> ani;
    auto live = [&ani]() {
        ani->Eat();
        ani->Show();
    };

    // 父类声明，子类实现这种多态，不能用 auto，编译器无法推导你的意图了
    ani = make_unique<Cat>();
    live();

    cout << "----\n";
    // reset 后，会自动析构原来的 raw-data, 但不是推荐写法
    // 可以直接赋值，会自动释放老的资源的
    // ani.reset(new Dog);
    ani = make_unique<Dog>();
    cout << "----\n";
    live();

    cout << "1+1=" << add(1, 1) << endl;
    cout << "1-1=" << sub(1, 1) << endl;

    return 0;
}