#ifndef TRANSACTIONS_INCLUDE_HASH_TABLE_HASH_TABLE_H_
#define TRANSACTIONS_INCLUDE_HASH_TABLE_HASH_TABLE_H_

#include <functional>
#include <vector>
#include <list>
#include <numeric>
#include <fstream>
#include <sstream>

#include "common/storage_interface.h"
#include "common/timer.h"

namespace s21
{

template<class Key, class Tp = Value, class Hash = std::hash<Key>>
class HashTable : public KeyValueStorageInterface<Key, Tp>
{
public:
    using key_type = typename KeyValueStorageInterface<Key, Tp>::key_type;
    using mapped_type = typename KeyValueStorageInterface<Key, Tp>::mapped_type;
    using size_type = typename KeyValueStorageInterface<Key, Tp>::size_type;

private:
    struct Entry final
    {
        key_type key{};
        mapped_type value{};

        explicit Entry(key_type key, mapped_type value)
            : key(key)
            , value(value)
        {}
    };

private:
    using List = std::list<Entry>;
    using Table = std::vector<List>;

public:
    HashTable() = default;

    bool Insert(const key_type& key, const mapped_type& value) override;
    mapped_type& GetValue(const key_type& key) override;
    bool Erase(const key_type& key) override;
    std::vector<std::pair<key_type, mapped_type>> ShowAll() override;

private:
    Entry* Find_(const key_type& key);
    void Rehash_();
    [[nodiscard]] size_type GetHashValue_(const key_type& value) const;
    [[nodiscard]] size_type GetNewTableIndex_(const key_type& value) const;

private:
    size_type table_size_{ 10 };
    size_type num_elements_{ 0 };
    Table table_{ 10 };
};

} // namespace s21

#include "hash_table.tpp"

#endif // TRANSACTIONS_INCLUDE_HASH_TABLE_HASH_TABLE_H_
