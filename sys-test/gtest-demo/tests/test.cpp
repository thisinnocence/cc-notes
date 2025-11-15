#include <gtest/gtest.h>

#include "add.h"

TEST(AddTest, PositiveNumbers) {
    EXPECT_EQ(add(2, 3), 5);
}

TEST(AddTest, NegativeNumbers) {
    EXPECT_EQ(add(-2, -3), -5);
}

TEST(AddTest, Zero) {
    EXPECT_EQ(add(0, 0), 0);
}

TEST(AddTest, Mixed) {
    EXPECT_EQ(add(10, -5), 5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
