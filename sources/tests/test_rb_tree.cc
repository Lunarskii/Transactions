#include "tests/test_rb_tree.h"

namespace Test
{

TEST_F(TreeAdding, Add_00)
{
    tree.addNode(24);
    tree.addNode(5);
    tree.addNode(1);
    tree.addNode(3);
    tree.addNode(8);
    EXPECT_TRUE(Compare(tree, {1, 3, 5, 8, 24}));

    tree.addNode(25);
    EXPECT_TRUE(Compare(tree, {1, 3, 5, 8, 24, 25}));

    tree.addNode(26);
    EXPECT_TRUE(Compare(tree, {1, 3, 5, 8, 24, 25, 26}));

    tree.addNode(2);
    EXPECT_TRUE(Compare(tree, {1, 2, 3, 5, 8, 24, 25, 26}));
}

TEST_F(TreeAdding, Add_01)
{
    auto [it, ok] = tree.addNode(1);
    EXPECT_TRUE(ok);
    EXPECT_EQ(*it, 1);

    auto [it2, ok2] = tree.addNode(1);
    EXPECT_FALSE(ok2);
    EXPECT_TRUE(it == it2);
}

TEST_F(TreeRemovingBase, Remove_00)
{
    ASSERT_TRUE(Compare(tree, {3, 5, 8, 24, 25, 26}));
    tree.removeNode(2);
    EXPECT_TRUE(Compare(tree, {3, 5, 8, 24, 25, 26}));
}

TEST_F(TreeRemovingBase, Remove_01)
{
    tree.addNode(1);
    tree.removeNode(8);
    EXPECT_TRUE(Compare(tree, {1, 3, 5, 24, 25, 26}));
}

TEST_F(TreeRemovingBase, Remove_02)
{
    tree.removeNode(26);
    EXPECT_TRUE(Compare(tree, {3, 5, 8, 24, 25}));
}

TEST_F(TreeRemovingBase, Remove_03)
{
    tree.addNode(7);
    tree.removeNode(3);
    EXPECT_TRUE(Compare(tree, {5, 7, 8, 24, 25, 26}));
}

TEST_F(TreeRemovingBase, Remove_04)
{
    tree.removeNode(3);
    EXPECT_TRUE(Compare(tree, {5, 8, 24, 25, 26}));
}

TEST_F(TreeRemovingBase, Remove_05)
{
    tree.removeNode(8);
    EXPECT_TRUE(Compare(tree, {3, 5, 24, 25, 26}));
}

TEST_F(TreeRemovingBase, Remove_06)
{
    tree.removeNode(5);
    EXPECT_TRUE(Compare(tree, {3, 8, 24, 25, 26}));
}

TEST_F(TreeRemovingAdv, Remove_00)
{
    EXPECT_TRUE(Compare(tree, {10,15, 25}));
    tree.addNode(7);
    tree.removeNode(25);
    EXPECT_TRUE(Compare(tree, {7, 10, 15}));
}

TEST_F(TreeRemovingAdv, Remove_01)
{
    tree.addNode(20);
    tree.removeNode(10);
    EXPECT_TRUE(Compare(tree, {15, 20, 25}));
}

TEST_F(TreeRemovingAdv, Remove_02)
{
    tree.addNode(5);
    tree.addNode(12);
    tree.addNode(2);
    tree.removeNode(25);
    EXPECT_TRUE(Compare(tree, {2, 5, 10, 12, 15}));
}

TEST_F(TreeRemovingAdv, Remove_03)
{
    tree.removeNode(25);
    tree.removeNode(10);
    tree.removeNode(15);
    EXPECT_TRUE(tree.isEmpty());
}

} // namespace Test
