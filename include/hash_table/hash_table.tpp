#ifndef TRANSACTIONS_INCLUDE_HASH_TABLE_HASH_TABLE_TPP_
#define TRANSACTIONS_INCLUDE_HASH_TABLE_HASH_TABLE_TPP_

namespace s21
{

template<class Key, class Tp, class Hash>
bool HashTable<Key, Tp, Hash>::Insert(const key_type& key, const mapped_type& value)
{
    if (!Find_(key))
    {
        table_[GetNewTableIndex_(key)].emplace_back(key, value);
        ++num_elements_;
        if (num_elements_ == table_size_)
        {
            Rehash_();
        }
        
        return true;
    }

    return false;
}

template<class Key, class Tp, class Hash>
typename HashTable<Key, Tp, Hash>::mapped_type& HashTable<Key, Tp, Hash>::GetValue(const key_type& key)
{
    if (auto entry = Find_(key); entry)
    {
        return entry->value;
    }

    throw std::runtime_error("The value was not found.");
}

template<class Key, class Tp, class Hash>
bool HashTable<Key, Tp, Hash>::Erase(const key_type& key)
{
    auto& list = table_[GetNewTableIndex_(key)];
    auto it = std::find_if(list.begin(), list.end(), [&key](auto& x)
    {
        return x.key == key;
    });

    if (it != list.end())
    {
        table_[GetNewTableIndex_(key)].erase(it);
        --num_elements_;
        return true;
    }

    return false;
}

template<class Key, class Tp, class Hash>
std::vector<std::pair<typename HashTable<Key, Tp, Hash>::key_type, typename HashTable<Key, Tp, Hash>::mapped_type>> HashTable<Key, Tp, Hash>::ShowAll()
{
    std::vector<std::pair<key_type, mapped_type>> entries;

    for (const auto& list : table_)
    {
        for (const auto& entry : list)
        {
            entries.push_back({ entry.key, entry.value });
        }
    }

    return entries;
}

template<class Key, class Tp, class Hash>
typename HashTable<Key, Tp, Hash>::Entry* HashTable<Key, Tp, Hash>::Find_(const key_type& key)
{
    auto& list = table_[GetNewTableIndex_(key)];
    auto it = std::find_if(list.begin(), list.end(), [&key](auto& x)
    {
        return x.key == key;
    });

    if (it != list.end())
    {
        return &(*it);
    }

    return nullptr;
}

template<class Key, class Tp, class Hash>
void HashTable<Key, Tp, Hash>::Rehash_()
{
    table_size_ *= 2;
    num_elements_ = 0;
    auto old_table = std::move(table_);
    table_ = Table{table_size_};

    for (const auto& list : old_table)
    {
        for (const auto& entry : list)
        {
            Insert(entry.key, entry.value);
        }
    }
}

template<class Key, class Tp, class Hash>
typename HashTable<Key, Tp, Hash>::size_type HashTable<Key, Tp, Hash>::GetHashValue_(const key_type& value) const
{
    return Hash{}(value);
}

template<class Key, class Tp, class Hash>
typename HashTable<Key, Tp, Hash>::size_type HashTable<Key, Tp, Hash>::GetNewTableIndex_(const key_type& value) const
{
    return GetHashValue_(value) % table_size_;
}

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_HASH_TABLE_HASH_TABLE_TPP_
