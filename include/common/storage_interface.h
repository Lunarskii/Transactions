#ifndef TRANSACTIONS_INCLUDE_COMMON_STORAGE_INTERFACE_H_
#define TRANSACTIONS_INCLUDE_COMMON_STORAGE_INTERFACE_H_

#include <filesystem>
#include <vector>
#include "storage_struct.h"

namespace s21
{

template<class Key, class Tp>
class KeyValueStorageInterface
{
public:
    using key_type = Key;
    using mapped_type = Tp;
    using size_type = std::size_t;

public:
    virtual ~KeyValueStorageInterface() = default;
    
    virtual bool Insert(const key_type& key, const mapped_type& value) = 0;
    virtual mapped_type& GetValue(const key_type& key) = 0;
    virtual bool Erase(const key_type& key) = 0;
    virtual std::vector<std::pair<key_type, mapped_type>> ShowAll() = 0;
};

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_COMMON_STORAGE_INTERFACE_H_
