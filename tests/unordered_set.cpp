
#include <set.hpp>

#include <gtest/gtest.h>


TEST(InitializeTest, Empty) {

    UnorderedSet<int> set{};

    EXPECT_EQ(set.size(), 0);
    EXPECT_TRUE(set.empty());
}

TEST(InitializeTest, Reference) {

    UnorderedSet<int> set{ 1, 2, 3 };

    EXPECT_EQ(set.size(), 3);
    EXPECT_FALSE(set.empty());
}


TEST(InitializeTest, Move) {

    UnorderedSet<int> set{ std::move(1) };

    EXPECT_EQ(set.size(), 1);
    EXPECT_FALSE(set.empty());
}
