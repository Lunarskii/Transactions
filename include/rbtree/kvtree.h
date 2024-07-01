#ifndef TRANSACTIONS_INCLUDE_RBTREE_KVTREE_H_
#define TRANSACTIONS_INCLUDE_RBTREE_KVTREE_H_

#include "common/storage_interface.h"
#include "rbtree.h"

namespace s21
{

template<class Key, class Value = Value>
class SelfBalancingBinarySearchTree: public KeyValueStorageInterface<Key, Value> {
 public:
  template<typename T1, typename T2>
  struct CompareByFirst {
    bool operator()(const std::pair<T1, T2>& a, const std::pair<T1, T2>& b) const {
      return a.first < b.first;
    }
  };

  using value_type = std::pair<const Key, Value>;
  using tree_type = s21_utils::rbTree<value_type, CompareByFirst<const Key, Value>>;
  using iterator = typename tree_type::iterator;
  using const_iterator = typename tree_type::const_iterator;


  bool Insert(const Key& key,
    const Value& value) override
  {
    auto [it, ok] = tree.addNode(std::make_pair(key, value));
    return ok;
  }

  Value& GetValue(const Key& key) override {
    auto [it, ok] = tree.FindKey(value_type(key, {}));
    if (!ok) throw std::runtime_error("no such key");
    return it->second;
  }
  bool Erase(const Key& key) override
  {
    auto num = tree.removeNode(value_type(key,{}));
    return num == 1;
  }
  std::vector<std::pair<Key, Value>> ShowAll() override {
    std::vector<std::pair<Key, Value>> result;
    for (auto const &node: tree)
    {
      result.push_back(node);
    }
    return result;
  }
 private:
   tree_type tree;
};

} // namespace s21

#endif  // TRANSACTIONS_INCLUDE_RBTREE_KVTREE_H_
