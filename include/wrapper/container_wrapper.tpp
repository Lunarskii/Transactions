#ifndef TRANSACTIONS_INCLUDE_WRAPPER_CONTAINER_WRAPPER_TPP_
#define TRANSACTIONS_INCLUDE_WRAPPER_CONTAINER_WRAPPER_TPP_

#include "container_wrapper.h"

namespace s21
{

template<class Container>
ContainerWrapper<Container>::ContainerWrapper()
    : container_(std::make_unique<Container>())
{}

template<class Container>
ContainerWrapper<Container>::ContainerWrapper(Container* container)
    : container_(container)
{}

template<class Container>
bool ContainerWrapper<Container>::Insert(const key_type& key, const mapped_type& value, int64_t life_time)
{
    RemoveIfExpired(key);
    auto result = container_->Insert(key, value);

    if (result && life_time > 0)
    {
        timer_storage_.emplace(key, life_time);
        timer_storage_.at(key).Start();
    }

    return result;
}


template<class Container>
typename ContainerWrapper<Container>::mapped_type ContainerWrapper<Container>::GetValue(const key_type& key)
{
    RemoveIfExpired(key);
    return container_->GetValue(key);
}

template<class Container>
bool ContainerWrapper<Container>::Exists(const key_type& key)
{
    RemoveIfExpired(key);

    try
    {
        container_->GetValue(key);
    }
    catch (const std::runtime_error& e)
    {
        return false;
    }

    return true;
}

template<class Container>
bool ContainerWrapper<Container>::Erase(const key_type& key)
{
    if (auto it = timer_storage_.find(key); it != timer_storage_.end())
    {
        auto expired = it->second.IsExpired();

        timer_storage_.erase(it);
        if (expired)
        {
            container_->Erase(key);
            return false;
        }
    }
    return container_->Erase(key);
}

template<class Container>
bool ContainerWrapper<Container>::Update(const key_type& key, const mapped_type& value)
{
    RemoveIfExpired(key);

    if (!value.IsDefault())
    {
        try
        {
            container_->GetValue(key) = value;
            return true;
        }
        catch (const std::runtime_error& e)
        {

        }
    }

    return false;
}

template<class Container>
std::vector<typename ContainerWrapper<Container>::key_type> ContainerWrapper<Container>::Keys()
{
    RemoveAllExpired();
    std::vector<key_type> keys;

    for (const auto& [key, value] : container_->ShowAll())
    {
        keys.push_back(key);
    }

    return keys;
}

template<class Container>
bool ContainerWrapper<Container>::Rename(const key_type& current_key, const key_type& new_key)
{
    if (current_key != new_key)
    {
        RemoveIfExpired(current_key);
        RemoveIfExpired(new_key);

        try
        {
            if (!Exists(new_key))
            {
                auto value = container_->GetValue(current_key);
                container_->Erase(current_key);
                container_->Insert(new_key, value);

                auto timer_it = timer_storage_.find(current_key);
                if (timer_it != timer_storage_.end())
                {
                    auto timer = timer_it->second;
                    timer_storage_.erase(timer_it);
                    timer_storage_.insert({ new_key, timer });
                }

                return true;
            }
        }
        catch (const std::runtime_error& e)
        {

        }
    }

    return false;
}

template<class Container>
typename ContainerWrapper<Container>::size_type ContainerWrapper<Container>::TTL(const key_type& key)
{
    RemoveIfExpired(key);

    if (Exists(key))
    {
        if (auto it = timer_storage_.find(key); it != timer_storage_.end())
        {
            return it->second.GetRemainingTime();
        }
        throw std::runtime_error("The key does not have a timestamp.");
    }

    throw std::runtime_error("The key was not found.");
}

template<class Container>
std::vector<typename ContainerWrapper<Container>::key_type> ContainerWrapper<Container>::Find(const mapped_type& value)
{
    RemoveAllExpired();
    std::vector<key_type> keys;

    for (const auto& kv : container_->ShowAll())
    {
        if (kv.second == value)
        {
            keys.push_back(kv.first);
        }
    }

    return keys;
}

template<class Container>
std::vector<typename ContainerWrapper<Container>::mapped_type> ContainerWrapper<Container>::ShowAll()
{
    RemoveAllExpired();
    std::vector<mapped_type> values;

    for (const auto& [key, value] : container_->ShowAll())
    {
        values.push_back(value);
    }

    return values;
}

template<class Container>
typename ContainerWrapper<Container>::size_type ContainerWrapper<Container>::Upload(const std::filesystem::path& path)
{
    if (std::filesystem::is_directory(path) || !std::filesystem::exists(path))
    {
        throw std::runtime_error("File '" + path.string() + "' not exists.");
    }

    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open the file.");
    }

    size_type num_entries = 0;
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        key_type key;
        mapped_type value;
        size_type life_time = 0;

        try
        {
            iss >> key >> value;
            if (!iss.fail() && !iss.eof())
            {
                iss >> life_time;
            }
        }
        catch (const std::exception& e)
        {
            continue;
        }

        if (!iss.fail() && Insert(key, value, life_time))
        {
            ++num_entries;
        }
    }

    return num_entries;
}

template<class Container>
typename ContainerWrapper<Container>::size_type ContainerWrapper<Container>::Export(const std::filesystem::path& path)
{
    if (std::filesystem::is_directory(path))
    {
        throw std::runtime_error(path.string() + " is directory.");
    }

    std::ofstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open the file.");
    }

    size_type num_entries = 0;

    for (const auto& [key, value] : container_->ShowAll())
    {
        if (!RemoveIfExpired(key))
        {
            ++num_entries;
            file << key << " " << value;
            if (auto it = timer_storage_.find(key); it != timer_storage_.end())
            {
                file << " " << it->second.GetRemainingTime();
            }
            file << std::endl;
        }
    }

    return num_entries;
}

template<class Container>
bool ContainerWrapper<Container>::RemoveIfExpired(const key_type& key)
{
    if (auto it = timer_storage_.find(key); it != timer_storage_.end() && it->second.IsExpired())
    {
        timer_storage_.erase(it);
        container_->Erase(key);
        return true;
    }

    return false;
}

template<class Container>
void ContainerWrapper<Container>::RemoveAllExpired()
{
    for (auto it = timer_storage_.begin(); it != timer_storage_.end(); )
    {
        if (it->second.IsExpired())
        {
            container_->Erase(it->first);
            it = timer_storage_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
    
} // namespace s21

#endif // TRANSACTIONS_INCLUDE_WRAPPER_CONTAINER_WRAPPER_TPP_
