#include <iostream>
#include <cstring>
#include <utility>  // std::move

class MyString {
    char* data;
    size_t length;

public:
    // 普通构造函数
    MyString(const char* str = "") {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    }

    // 析构函数
    ~MyString() {
        delete[] data;
    }

    // 拷贝构造函数（深拷贝）
    MyString(const MyString& other) {
        length = other.length;
        data = new char[length + 1];
        strcpy(data, other.data);
    }

    // 移动构造函数（窃取资源）
    MyString(MyString&& other) noexcept {
        std::cout << "***move assignment func ***\n";
        data = other.data;     // 接管指针
        length = other.length;
        other.data = nullptr;  // 置空原指针
        other.length = 0;
    }

    // 移动赋值运算符
    MyString& operator=(MyString&& other) noexcept {
        std::cout << "***move assignment = ***\n";
        if (this != &other) {
            delete[] data;         // 释放当前资源
            data = other.data;     // 接管新指针
            length = other.length;
            other.data = nullptr;  // 置空原指针
            other.length = 0;
        }
        return *this;
    }

    // 打印内容
    void print() const {
        std::cout << (data ? data : "nullptr") << "\n";
    }
};

int main() {
    MyString s1 = "Hello";  // 普通构造

    MyString s2 = std::move(s1);  // 移动构造
    std::cout << "s1: ";
    s1.print();  // 输出 nullptr（资源已被转移）
    std::cout << "s2: ";
    s2.print();  // 输出 Hello

    MyString s3;
    s3 = std::move(s2);  // 移动赋值
    std::cout << "s2: ";
    s2.print();  // 输出 nullptr
    std::cout << "s3: ";
    s3.print();  // 输出 Hello
}
