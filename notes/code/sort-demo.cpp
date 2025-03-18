#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

struct Person {
    std::string name;
    int age;

    // 方法1：重载小于运算符
    bool operator<(const Person& other) const {
        return age < other.age;
    }
};

int main() {
    std::vector<Person> people = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};

    // 方法1：使用重载运算符排序
    std::sort(people.begin(), people.end());
    printf("Method 1: Sorted by overloaded operator, by age:\n");
    for (const auto& person : people) {
        printf("%s (%d)\n", person.name.c_str(), person.age);
    }

    // 方法2：使用自定义比较函数
    std::sort(people.begin(), people.end(), [](const Person& a, const Person& b) { return a.name < b.name; });
    printf("Method 2: Sorted by custom comparator, by name dictionary order:\n");
    for (const auto& person : people) {
        printf("%s (%d)\n", person.name.c_str(), person.age);
    }
    return 0;
}
