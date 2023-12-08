
#include <set.hpp>

#include <gtest/gtest.h>


TEST(InitializeTest, Empty) {
    UnorderedSet<int> set{};

    EXPECT_EQ(set.size(), 0);
    EXPECT_TRUE(set.empty());
}

TEST(InitializeTest, Reference) {
    std::initializer_list<int> ref{ 1, 2, 3 };

    UnorderedSet<int> set{ ref };

    EXPECT_EQ(set.size(), 3);
    EXPECT_FALSE(set.empty());
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 1);
    EXPECT_EQ(set.count(3), 1);
    EXPECT_EQ(set.count(12), 0);
}

TEST(InitializeTest, ReferenceEmpty) {
    std::initializer_list<int> ref{};

    UnorderedSet<int> set{ ref };

    EXPECT_EQ(set.size(), 0);
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.count(1), 0);
}


TEST(InitializeTest, Move) {
    UnorderedSet<int> set{ std::move(1) };

    EXPECT_EQ(set.size(), 1);
    EXPECT_FALSE(set.empty());
}

TEST(InitializeTest, MoveInitializerList) {
    std::initializer_list<int> ref{ 1, 2, 3 };

    UnorderedSet<int> set{ std::move(ref) };

    EXPECT_EQ(set.size(), 3);
    EXPECT_FALSE(set.empty());
    EXPECT_EQ(set.count(1), 1);
    EXPECT_EQ(set.count(2), 1);
    EXPECT_EQ(set.count(3), 1);
    EXPECT_EQ(set.count(12), 0);
}


TEST(InitializeTest, MoveEmpty) {
    std::initializer_list<int> ref{};

    UnorderedSet<int> set{ std::move(ref) };

    EXPECT_EQ(set.size(), 0);
    EXPECT_TRUE(set.empty());
}

TEST(InitializeTest, CopyConstructor) {
    UnorderedSet<int> set1{ 1, 2, 3 };
    EXPECT_EQ(set1.size(), 3);

    UnorderedSet<int> set2{ set1 };
    EXPECT_EQ(set2.size(), 3);

    set2.insert(5);
    EXPECT_EQ(set1.size(), 3);
    EXPECT_EQ(set2.size(), 4);
    EXPECT_EQ(set1.count(5), 0);
    EXPECT_EQ(set2.count(5), 1);

    set1.insert(6);
    EXPECT_EQ(set1.size(), 4);
    EXPECT_EQ(set2.size(), 4);
    EXPECT_EQ(set1.count(6), 1);
    EXPECT_EQ(set2.count(6), 0);
}

TEST(InitializeTest, MoveConstructor) {
    UnorderedSet<int> set1{ 1, 2, 3 };
    EXPECT_EQ(set1.size(), 3);

    UnorderedSet<int> set2{ std::move(set1) };
    EXPECT_EQ(set2.size(), 3);

    set2.insert(5);
    EXPECT_EQ(set1.size(), 0);
    EXPECT_EQ(set2.size(), 4);

    set1.insert(6);
    EXPECT_EQ(set1.size(), 1);
    EXPECT_EQ(set2.size(), 4);
}


TEST(OperationsTest, union) {
    UnorderedSet<int> set1{ 1, 2, 3 };
    EXPECT_EQ(set1.size(), 3);

    UnorderedSet<int> set2{ 1, 2, 3, 4 };
    EXPECT_EQ(set2.size(), 4);

    set1 += set2;

    EXPECT_EQ(set2.size(), 4);
    EXPECT_EQ(set1.size(), 4);

    EXPECT_EQ(set1.count(1), 1);
    EXPECT_EQ(set1.count(2), 1);
    EXPECT_EQ(set1.count(3), 1);
    EXPECT_EQ(set1.count(4), 1);
}


TEST(OperationsTest, without) {
    UnorderedSet<int> set1{ 1, 2, 3, 4 };
    EXPECT_EQ(set1.size(), 4);

    UnorderedSet<int> set2{ 1, 2, 3, 5 };
    EXPECT_EQ(set2.size(), 4);

    set1 -= set2;

    EXPECT_EQ(set2.size(), 4);
    EXPECT_EQ(set1.size(), 1);

    EXPECT_EQ(set1.count(1), 0);
    EXPECT_EQ(set1.count(2), 0);
    EXPECT_EQ(set1.count(3), 0);
    EXPECT_EQ(set1.count(4), 1);
    EXPECT_EQ(set1.count(5), 0);
}
