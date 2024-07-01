#ifndef TRANSACTIONS_INCLUDE_WRAPPER_CONTAINER_WRAPPER_H_
#define TRANSACTIONS_INCLUDE_WRAPPER_CONTAINER_WRAPPER_H_

#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "common/storage_interface.h"
#include "common/timer.h"

namespace s21
{

template<class Container>
class ContainerWrapper
{
public:
    using key_type = typename Container::key_type;
    using mapped_type = typename Container::mapped_type;
    using size_type = typename Container::size_type;

public:
    ContainerWrapper();
    explicit ContainerWrapper(Container* container);

    bool Insert(const key_type& key, const mapped_type& value, int64_t life_time);
    mapped_type GetValue(const key_type& key);
    bool Exists(const key_type& key);
    bool Erase(const key_type& key);
    bool Update(const key_type& key, const mapped_type& value);
    std::vector<key_type> Keys();
    bool Rename(const key_type& current_key, const key_type& new_key);
    size_type TTL(const key_type& key);
    std::vector<key_type> Find(const mapped_type& value);
    std::vector<mapped_type> ShowAll();
    size_type Upload(const std::filesystem::path& path);
    size_type Export(const std::filesystem::path& path);

private:
    bool RemoveIfExpired(const key_type& key);
    void RemoveAllExpired();

private:
    std::unique_ptr<Container> container_;
    std::unordered_map<key_type, Timer<>> timer_storage_;
};

} // namespace s21

#include "container_wrapper.tpp"

#endif // TRANSACTIONS_INCLUDE_WRAPPER_CONTAINER_WRAPPER_H_
