#ifndef TRANSACTIONS_INCLUDE_TESTS_TEST_B_PLUS_TREE_H_
#define TRANSACTIONS_INCLUDE_TESTS_TEST_B_PLUS_TREE_H_

#include "test_core.h"
#include "bpt/b_plus_tree.h"

namespace Test
{

struct BPlusTreeTestParams
{
    std::size_t order;
    std::vector<std::pair<std::string, Value>> std_dataset_identical_values{
            {"key0",       value1},
            {"key1",       value1},
            {"key2",       value1},
            {"key3",       value1},
            {"key4",       value1},
            {"key5",       value1},
            {"key6",       value1},
            {"key7",       value1},
            {"key8",       value1},
            {"key9",       value1},
            {"isolation",  value1},
            {"prospect",   value1},
            {"trustee",    value1},
            {"goat",       value1},
            {"sacrifice",  value1},
            {"appreciate", value1},
            {"land",       value1},
            {"elegant",    value1},
            {"viable",     value1},
            {"build",      value1},
    };
    std::vector<std::pair<std::string, Value>> std_dataset_different_values{
            {"key0",       value1},
            {"key1",       value2},
            {"key2",       value1},
            {"key3",       value2},
            {"key4",       value1},
            {"key5",       value2},
            {"key6",       value1},
            {"key7",       value2},
            {"key8",       value1},
            {"key9",       value2},
            {"isolation",  value1},
            {"prospect",   value2},
            {"trustee",    value1},
            {"goat",       value2},
            {"sacrifice",  value1},
            {"appreciate", value2},
            {"land",       value1},
            {"elegant",    value2},
            {"viable",     value1},
            {"build",      value2},
    };
    std::vector<std::pair<std::string, Value>> impossible_to_borrow_dataset{
            {"key0", value1},
            {"key1", value1},
            {"key2", value1},
            {"key3", value1},
            {"key4", value1},
    };
};

class BPlusTreeSuite : public ::testing::TestWithParam<BPlusTreeTestParams>
{
protected:
    void SetUp() override
    {
        bpt = new BPlusTree<std::string>(GetParam().order);
    }

    void TearDown() override
    {
        delete bpt;
    }

protected:
    BPlusTree<std::string>* bpt{ nullptr };
};

INSTANTIATE_TEST_SUITE_P(
        BPlusTreeOrder3,
        BPlusTreeSuite,
        ::testing::Values(BPlusTreeTestParams{3})
);

//INSTANTIATE_TEST_SUITE_P(
//        BPlusTreeOrder4,
//        BPlusTreeSuite,
//        ::testing::Values(BPlusTreeTestParams{4})
//);
//
//INSTANTIATE_TEST_SUITE_P(
//        BPlusTreeOrder5,
//        BPlusTreeSuite,
//        ::testing::Values(BPlusTreeTestParams{5})
//);
//
//INSTANTIATE_TEST_SUITE_P(
//        BPlusTreeOrder6,
//        BPlusTreeSuite,
//        ::testing::Values(BPlusTreeTestParams{6})
//);
//
//INSTANTIATE_TEST_SUITE_P(
//        BPlusTreeOrder7,
//        BPlusTreeSuite,
//        ::testing::Values(BPlusTreeTestParams{7})
//);

} // namespace Test

#endif // TRANSACTIONS_INCLUDE_TESTS_TEST_B_PLUS_TREE_H_
