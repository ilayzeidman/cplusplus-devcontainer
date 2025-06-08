#include <gtest/gtest.h>
#include "concepts/shared_pointer/shared_example.hpp"

TEST(SharedPointerTest, BasicUsage) {
    std::shared_ptr<SharedExample> examplePtr = std::make_shared<SharedExample>(42);
    ASSERT_NE(examplePtr, nullptr);
    EXPECT_EQ(examplePtr->getValue(), 42);

    // Test shared ownership
    std::shared_ptr<SharedExample> anotherPtr = examplePtr;
    EXPECT_EQ(anotherPtr->getValue(), 42);
    
    // Check reference count
    EXPECT_EQ(examplePtr.use_count(), 2);
}

TEST(SharedPointerTest, SharedPtrUsage) {
    auto ptr1 = std::make_shared<SharedExample>(5);
    auto ptr2 = ptr1;  // compteur partagé

    EXPECT_EQ(ptr1->getValue(), 5);
    EXPECT_EQ(ptr2->getValue(), 5);
    EXPECT_EQ(ptr1.use_count(), 2);  // compteur de référence
}