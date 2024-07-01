#ifndef TRANSACTIONS_INCLUDE_TESTS_TEST_HASH_TABLE_H_
#define TRANSACTIONS_INCLUDE_TESTS_TEST_HASH_TABLE_H_

#include "test_core.h"
#include "hash_table/hash_table.h"

namespace Test
{

struct HashTableParams
{
    std::vector<std::pair<std::string, Value>> std_dataset_identical_values{
            {"snail", value1},
            {"youth", value1},
            {"lily", value1},
            {"tease", value1},
            {"secretion", value1},
            {"portrait", value1},
            {"cultural", value1},
            {"overcharge", value1},
            {"folklore", value1},
            {"glare", value1},
            {"absence", value1},
            {"neighbour", value1},
            {"tycoon", value1},
            {"enhance", value1},
            {"tight", value1},
            {"install", value1},
            {"affect", value1},
            {"draw", value1},
            {"functional", value1},
            {"whip", value1},
    };
    std::vector<std::pair<std::string, Value>> std_dataset_different_values{
            {"snail", value1},
            {"youth", value2},
            {"lily", value1},
            {"tease", value2},
            {"secretion", value1},
            {"portrait", value2},
            {"cultural", value1},
            {"overcharge", value2},
            {"folklore", value1},
            {"glare", value2},
            {"absence", value1},
            {"neighbour", value2},
            {"tycoon", value1},
            {"enhance", value2},
            {"tight", value1},
            {"install", value2},
            {"affect", value1},
            {"draw", value2},
            {"functional", value1},
            {"whip", value2},
    };
};

class HashTableSuite : public ::testing::TestWithParam<HashTableParams>
{
protected:
    void SetUp() override
    {
        hash_table = new HashTable<std::string>();
    }

    void TearDown() override
    {
        delete hash_table;
    }

protected:
    HashTable<std::string>* hash_table{ nullptr };
};

INSTANTIATE_TEST_SUITE_P(
        HashTableStd,
        HashTableSuite,
        ::testing::Values(HashTableParams{})
);

} // namespace Test

#endif // TRANSACTIONS_INCLUDE_TESTS_TEST_HASH_TABLE_H_
