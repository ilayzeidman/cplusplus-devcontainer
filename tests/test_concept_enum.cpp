#include <gtest/gtest.h>
#include "concepts/enum/enum.hpp"

TEST(EnumTest, IsPrimaryColor) {
    EXPECT_TRUE(isPrimaryColor(Color::Red));
    EXPECT_TRUE(isPrimaryColor(Color::Green));
    EXPECT_TRUE(isPrimaryColor(Color::Blue));
    EXPECT_FALSE(isPrimaryColor(static_cast<Color>(100))); // Testing an invalid color
}