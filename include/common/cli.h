#ifndef TRANSACTIONS_INCLUDE_COMMON_CLI_H_
#define TRANSACTIONS_INCLUDE_COMMON_CLI_H_

#include <iostream>

#include "command_invoker.h"
#include "wrapper/container_wrapper.h"
#include "hash_table/hash_table.h"
#include "bpt/b_plus_tree.h"
#include "research.h"
#include "rbtree/kvtree.h"

namespace s21
{

class CLI
{
public:
    CLI() = default;
    int Run();

private:
    using storage_type = KeyValueStorageInterface<std::string, Value>;
    using wrapper_type = ContainerWrapper<storage_type>;
    using hash_table = HashTable<std::string>;
    using b_plus_tree = BPlusTree<std::string>;
    using rb_tree = SelfBalancingBinarySearchTree<std::string>;

private:
    static void CleanInputStream_();
    bool MainSelector_();
    bool SelectStorageType_();
    bool SelectFunction_();
    void Research_();

private:
    std::unique_ptr<wrapper_type> storage_;
    std::unique_ptr<CommandInvoker<wrapper_type>> command_ = std::make_unique<CommandInvoker<wrapper_type>>();
};

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_COMMON_CLI_H_
