#include "tests/test_hash_table.h"

namespace Test
{

template<class Key>
void InsertKeys(HashTable<Key>* hash_table, const std::vector<std::pair<Key, Value>>& keys, const bool expect)
{
    for (const auto& kv : keys)
    {
        EXPECT_EQ(hash_table->Insert(kv.first, kv.second), expect);
    }
}

TEST_P(HashTableSuite, Rehash)
{
    InsertKeys(hash_table, GetParam().std_dataset_identical_values, true);
    for (const auto& kv : GetParam().std_dataset_identical_values)
    {
        EXPECT_NO_THROW(hash_table->GetValue(kv.first));
    }
}

} // namespace Test
