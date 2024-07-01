#ifndef TRANSACTIONS_INCLUDE_TESTS_TEST_CONTAINER_WRAPPER_H_
#define TRANSACTIONS_INCLUDE_TESTS_TEST_CONTAINER_WRAPPER_H_

#include <cxxabi.h>

#include "test_core.h"
#include "wrapper/container_wrapper.h"
#include "hash_table/hash_table.h"
#include "bpt/b_plus_tree.h"
#include "rbtree/kvtree.h"

namespace Test
{

struct ContainerWrapperParams
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

class NameGenerator
{
public:
    template <typename T>
    static std::string GetName(int)
    {
        if constexpr (std::is_same_v<T, HashTable<std::string>>) return "HashTable";
        if constexpr (std::is_same_v<T, BPlusTree<std::string>>) return "BPlusTree";
        if constexpr (std::is_same_v<T, SelfBalancingBinarySearchTree<std::string>>) return "RBTree";

        return "UnnamedType";
    }
};

template<class Container>
class ContainerWrapperSuite : public ::testing::Test
{
protected:
    void SetUp() override
    {
        container_wrapper = new ContainerWrapper<Container>();
    }

    void TearDown() override
    {
        delete container_wrapper;
    }

    [[nodiscard]] std::string GetTypeName() const
    {
        return NameGenerator::GetName<Container>(0);
    }

protected:
    ContainerWrapper<Container>* container_wrapper{ nullptr };
    ContainerWrapperParams params_;
};

using ContainerTypes = ::testing::Types<HashTable<std::string>, BPlusTree<std::string>, SelfBalancingBinarySearchTree<std::string>>;
TYPED_TEST_SUITE(ContainerWrapperSuite, ContainerTypes, NameGenerator);

} // namespace Test

#endif // TRANSACTIONS_INCLUDE_TESTS_TEST_CONTAINER_WRAPPER_H_
