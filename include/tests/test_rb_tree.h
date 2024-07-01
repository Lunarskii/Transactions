#ifndef TRANSACTIONS_INCLUDE_TESTS_TEST_RB_TREE_H_
#define TRANSACTIONS_INCLUDE_TESTS_TEST_RB_TREE_H_

#include "test_core.h"
#include "rbtree/rbtree.h"

namespace Test
{

template <class T>
bool Compare(const s21_utils::rbTree<T>& tree, const std::vector<T>& expected)
{
    return std::equal(tree.begin(), tree.end(), expected.begin());
}

class TreeAdding: public ::testing::Test
{
    void SetUp() override {};
    void TearDown() override {};
protected:
    s21_utils::rbTree<int> tree;
};

class TreeBase: public ::testing::Test
{
    void SetUp() override
    {
        tree = {1, 2, 3};
    };
    void TearDown() override {};
protected:
    s21_utils::rbTree<int> tree;
    s21_utils::rbTree<int> second_tree;
};

class TreeRemovingBase : public ::testing::Test
{
    void SetUp() override
    {
        tree.addNode({24, 5, 3, 8, 25, 26});
    };
    void TearDown() override {};
protected:
    s21_utils::rbTree<int> tree;
};

class TreeRemovingAdv : public ::testing::Test
{
    void SetUp() override
    {
        tree.addNode({15, 10, 25});
    };
    void TearDown() override {};
protected:
    s21_utils::rbTree<int> tree;
};

} // namespace Test

#endif  // TRANSACTIONS_INCLUDE_TESTS_TEST_RB_TREE_H_
