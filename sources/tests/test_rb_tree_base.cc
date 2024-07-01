#include "tests/test_rb_tree.h"

namespace Test
{

TEST_F(TreeBase, ctor)
{
    ASSERT_TRUE(Compare(tree, {1, 2, 3}));
    ASSERT_TRUE(second_tree.isEmpty() && second_tree.begin()==second_tree.end());
    second_tree = tree;
    tree = tree;
    ASSERT_TRUE(Compare(tree, {1, 2, 3}));
    ASSERT_TRUE(Compare(second_tree, {1, 2, 3}));
}

TEST_F(TreeBase, other)
{
    auto move_test = s21_utils::rbTree<int>{1, 2, 3};
    ASSERT_TRUE(Compare(move_test, {1, 2, 3}));
    tree.clearTree();
    ASSERT_TRUE(tree.isEmpty() && second_tree.begin()==second_tree.end());
}

TEST_F(TreeBase, iter)
{
    second_tree = {3, 5, 8, 24, 25, 26};
    std::vector<int> res;
    auto [it, ok] = second_tree.FindKey(26);
    ASSERT_TRUE(ok);

    for (; it != second_tree.end(); --it)
    {
        res.push_back(*it);
    }
    EXPECT_TRUE(res == std::vector<int>({26, 25, 24, 8, 5, 3}));
}

} // namespace Test
