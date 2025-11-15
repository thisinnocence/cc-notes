#include <algorithm>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    vector<int> va = {1, 2, 5, 0, 3};

    // 泛型 lambda, 入参是 auto, (C++14)
    auto show = [&va]() {
        for (const auto& e : va) {
            cout << e << " ";
        }
        cout << endl;
    };

    // sort 默认的比较是 operator<
    // 自定义比较函数时：
    //   - 对于基本数据类型，可以用 auto （C++14）, 值传递效率也高
    //   - 如果是结构体对象，就要用 const auto&, 用常量引用避免大量拷贝
    sort(va.begin(), va.end(), [](auto a, auto b) { return a < b; });
    show();

    // 删除满足条件元素，内存操作性能更高
    auto new_end = remove_if(va.begin(), va.end(), [](const auto& e) { return e <= 1; });
    va.erase(new_end, va.end());

    sort(va.begin(), va.end(), [](const auto& a, const auto& b) { return a > b; });
    show();

    auto show_map = [](const auto& m) {
        for (const auto& [key, value] : m) {  // C++17 结构化绑定
            std::cout << key << ": " << value << std::endl;
        }
        cout << endl;
    };

    // map 红黑数，内部存储已经是有序(默认从小到大)
    map<int, string> ma = {
        {1, "aa"},
        {5, "cc"},
        {2, "bb"},
    };
    // 结构化绑定 (C++17)
    show_map(ma);

    auto cmp = [](const auto& a, const auto& b) { return a > b; };
    map<int, string, decltype(cmp)> mma(cmp);
    mma = {
        {1, "aa"},
        {5, "cc"},
        {2, "bb"},
    };
    show_map(mma);

    return 0;
}