#include <iostream>
#include <vector>
#include <array>
#include <cstdio>
#include <cstring>
#include <cctype>

namespace ArrayParser {
    void SkipSpaces(const char*& s) {
        while (*s && std::isspace(static_cast<unsigned char>(*s))) ++s;
    }

    bool ParseArray(std::array<int, 5>& arr, const char*& s) {
        SkipSpaces(s);
        if (*s != '[') return false;
        ++s;

        for (int i = 0; i < 5; ++i) {
            SkipSpaces(s);

            if (!std::isdigit(*s)) return false;
            int val = 0;
            while (std::isdigit(*s)) {
                val = val * 10 + (*s - '0');
                ++s;
            }
            arr[i] = val;

            SkipSpaces(s);
            if (i < 4) {
                if (*s != ',') return false;
                ++s;
            }
        }

        SkipSpaces(s);
        if (*s != ']') return false;
        ++s;
        return true;
    }

    bool Parse(const std::string& input, std::vector<std::array<int, 5>>& result) {
        const char* s = input.c_str();
        SkipSpaces(s);
        if (*s != '[') return false;
        ++s;

        for (;;) {
            SkipSpaces(s);
            if (*s == ']') break;

            std::array<int, 5> arr;
            if (!ParseArray(arr, s)) return false;
            result.push_back(arr);

            SkipSpaces(s);
            if (*s == ',') {
                ++s;
            } else if (*s == ']') {
                break;
            } else {
                return false;
            }
        }

        ++s;
        SkipSpaces(s);
        return *s == '\0';
    }
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
        bool ok = ArrayParser::Parse(testCases[i].input, result);
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
