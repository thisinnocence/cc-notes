#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "add.h"
#include "base.h"

using ::testing::Return;

// Mock 对象，用于模拟 get_base() 的行为
class MockBase {
public:
    // MOCK_METHOD 是宏，会自动生成模拟方法的实现
    // 被调用时会记录调用次数，返回通过 willOnce(Return(...)) 设置的值
    // 参数说明：
    // 1. 返回类型：int
    // 2. 方法名：GetBase
    // 3. 参数列表：() 表示无参数
    MOCK_METHOD(int, GetBase, ());
};

// 全局指针，由测试时的 shim 使用，指向当前活跃的 Mock 实例
static MockBase* g_mock_base = nullptr;

// 测试用的自由函数 get_base() 的实现，会转发到 gmock 的 Mock 对象。
// 这样可以在不改生产代码的情况下控制 get_base() 的返回值与调用期望。
int get_base() {
    if (g_mock_base) {
        return g_mock_base->GetBase();
    }
    return 0;
}

// 测试：期望 get_base() 被调用一次并返回 10，add() 应把它加进去
TEST(AddTest, GetBaseCalledOnceAndReturns10) {
    MockBase mock;
    g_mock_base = &mock;  // 将 shim 指向我们的 mock 实例

    // clang-format off
    // 期望 get_base() 正好被调用一次并返回 10
    EXPECT_CALL(mock, GetBase())
        .Times(1)
        .WillOnce(Return(10));
    // clang-format on

    int res = add(2, 3);  // add 内部会调用 get_base()
    EXPECT_EQ(res, 2 + 3 + 10);

    g_mock_base = nullptr;  // 清理全局指针，避免影响其他测试
}

// 测试：期望 get_base() 被调用多次，每次返回不同的值
TEST(AddTest, GetBaseCalledMultipleTimesWithDifferentValues) {
    MockBase mock;
    g_mock_base = &mock;

    // clang-format off
    // 期望 get_base() 被调用两次：第一次返回 5，第二次返回 15
    EXPECT_CALL(mock, GetBase())
        .Times(2)
        .WillOnce(Return(5))
        .WillOnce(Return(15));
    // clang-format on

    int res1 = add(1, 1);  // 第一次调用 get_base()，返回 5，结果是 1+1+5=7
    EXPECT_EQ(res1, 7);

    int res2 = add(2, 2);  // 第二次调用 get_base()，返回 15，结果是 2+2+15=19
    EXPECT_EQ(res2, 19);

    g_mock_base = nullptr;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}