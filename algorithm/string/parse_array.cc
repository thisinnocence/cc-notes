#include <iostream>
#include <vector>
#include <array>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <charconv>

bool Parse(const std::string& input, std::vector<std::array<int, 5>>& res) {
    const char* s = input.c_str();
    const char* end = s + input.size();

    auto skip = [&]() { while (s < end && std::isspace(*s)) ++s; };

    auto parse5 = [&](std::array<int, 5>& arr) {
        skip(); if (s >= end || *s++ != '[') return false;
        for (int i = 0; i < 5; ++i) {
            skip(); auto [p, err] = std::from_chars(s, end, arr[i]);
            if (err != std::errc{}) return false; s = p;
            skip(); if (i < 4) { if (s >= end || *s++ != ',') return false; }
        }
        skip(); if (s >= end || *s++ != ']') return false;
        return true;
    };

    skip(); if (s >= end || *s++ != '[') return false;
    for(;;) {
        skip(); if (s >= end || *s == ']') break;
        std::array<int, 5> arr; if (!parse5(arr)) return false; res.push_back(arr);
        skip(); if (s >= end) return false;
        if (*s == ',') ++s;
        else if (*s != ']') return false;
    }
    skip(); if (s >= end || *s++ != ']') return false;
    skip(); return s == end;
}

void TestParse() {
    struct TestCase {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> testCases = {
        // ✅ 合法输入
        {"[[1,2,3,4,5],[6,7,8,9,10]]", true},
        {"[[1,2,3,4,5],[6,7,8,9,10],[111,3,355, 4,  6]]", true},
        {"[]", true},

        // ❌ 非法：额外逗号
        {"[[1,2,3,4,5,,],[6,7,8,9,10]]", false},

        // ❌ 非法：子数组少一个元素
        {"[[1,2,3,4],[6,7,8,9,10]]", false},

        // ❌ 非法：子数组多一个元素
        {"[[1,2,3,4,5,6],[6,7,8,9,10]]", false},

        // ❌ 非法：非数字
        {"[[1,2,3,4,a],[6,7,8,9,10]]", false},

        // ❌ 非法：结尾多一个逗号
        {"[[1,2,3,4,5],[6,7,8,9,10]],", false},

        // ❌ 非法：顶层不以中括号包裹
        {"1,2,3,4,5", false}
    };

    for (size_t i = 0; i < testCases.size(); ++i) {
        std::vector<std::array<int, 5>> result;
        bool ok = Parse(testCases[i].input, result);
        std::cout << "Test case " << i + 1 << ": " 
                  << (ok == testCases[i].expected ? "✅ Passed" : "❌ Failed")
                  << "\n";
        // Print the result if passed
        if (ok) {
            std::cout << "Parsed successfully:\n";
            for (const auto& arr : result) {
                for (size_t j = 0; j < arr.size(); ++j) {
                    std::cout << arr[j];
                    if (j < arr.size() - 1) std::cout << ",";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        } 
    }
}

int main()
{
    TestParse();
}
