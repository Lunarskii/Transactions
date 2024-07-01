#include "tests/test_b_plus_tree.h"

namespace Test
{

template<class Key>
void InsertKeys(BPlusTree<Key>* bpt, const std::vector<std::pair<Key, Value>>& keys, const bool expect)
{
    for (const auto& kv : keys)
    {
        EXPECT_EQ(bpt->Insert(kv.first, kv.second), expect);
    }
}

template<class Key>
void EraseKeys(BPlusTree<Key>* bpt, const std::vector<std::pair<Key, Value>>& keys, const bool expect)
{
    for (const auto& kv : keys)
    {
        EXPECT_EQ(bpt->Erase(kv.first), expect);
    }
}

TEST(BPlusTreeSuite_NP, Constructor_OrderLargerThan2)
{
    EXPECT_NO_THROW(BPlusTree<std::string> bpt{3});
    EXPECT_NO_THROW(BPlusTree<std::string> bpt{1000});
}

TEST(BPlusTreeSuite_NP, Constructor_OrderLessThan3)
{
    EXPECT_ANY_THROW(BPlusTree<std::string> bpt{0});
    EXPECT_ANY_THROW(BPlusTree<std::string> bpt{1});
    EXPECT_ANY_THROW(BPlusTree<std::string> bpt{2});
}

TEST_P(BPlusTreeSuite, Erase_LeafNode_LargerThanMinSize)
{
    InsertKeys(bpt, GetParam().std_dataset_identical_values, true);
    EXPECT_TRUE(bpt->Erase("appreciate"));
}

TEST_P(BPlusTreeSuite, Erase_LeafNode_PossibleToBorrow)
{
    InsertKeys(bpt, GetParam().std_dataset_identical_values, true);
    EXPECT_TRUE(bpt->Erase("appreciate"));
    EXPECT_TRUE(bpt->Erase("build"));
    EXPECT_TRUE(bpt->Erase("key1"));
}

TEST_P(BPlusTreeSuite, Erase_LeafNode_ImpossibleToBorrow_LeftSubtree)
{
    InsertKeys(bpt, GetParam().impossible_to_borrow_dataset, true);
    EXPECT_TRUE(bpt->Erase("key0"));
}

TEST_P(BPlusTreeSuite, Erase_LeafNode_ImpossibleToBorrow_RightSubtree)
{
    InsertKeys(bpt, GetParam().impossible_to_borrow_dataset, true);
    EXPECT_TRUE(bpt->Erase("key4"));
    EXPECT_TRUE(bpt->Erase("key2"));
}

TEST_P(BPlusTreeSuite, Erase_LeafNodeWithInternalNode_LargerThanMinSize)
{
    InsertKeys(bpt, GetParam().std_dataset_identical_values, true);
    EXPECT_TRUE(bpt->Erase("trustee"));
}

TEST_P(BPlusTreeSuite, Erase_LeafNodeWithInternalNode_PossibleToBorrow)
{
    InsertKeys(bpt, GetParam().std_dataset_identical_values, true);
    EXPECT_TRUE(bpt->Erase("key6"));
}

TEST_P(BPlusTreeSuite, Erase_LeafNodeWithInternalNode_ImpossibleToBorrow_LeftSubtree)
{
    InsertKeys(bpt, GetParam().impossible_to_borrow_dataset, true);
    EXPECT_TRUE(bpt->Erase("key1"));
}

TEST_P(BPlusTreeSuite, Erase_LeafNodeWithInternalNode_ImpossibleToBorrow_RightSubtree)
{
    InsertKeys(bpt, GetParam().impossible_to_borrow_dataset, true);
    EXPECT_TRUE(bpt->Erase("key4"));
    EXPECT_TRUE(bpt->Erase("key3"));
}

} // namespace Test
