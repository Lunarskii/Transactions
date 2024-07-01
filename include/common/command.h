#ifndef TRANSACTIONS_INCLUDE_COMMON_COMMAND_H_
#define TRANSACTIONS_INCLUDE_COMMON_COMMAND_H_

#include "common/storage_interface.h"

namespace s21::cmd
{

template<class Container>
class Command
{
public:
    using key_type = typename Container::key_type;
    using mapped_type = typename Container::mapped_type;

    virtual ~Command() = default;
    virtual void Execute(Container& storage) = 0;
};

template<class Container>
class SetCommand : public Command<Container>
{
public:
    using key_type = typename Command<Container>::key_type;
    using mapped_type = typename Command<Container>::mapped_type;
    
    explicit SetCommand(std::istream& is)
    {
        is >> key_;
        is >> value_;

        std::string ex;
        is >> ex;
        std::transform(ex.begin(), ex.end(), ex.begin(), [](auto c)
        {
            return std::toupper(c);
        });
        if (ex == "EX")
        {
            is >> life_time_;
        }
    }

    void Execute(Container& storage) override
    {
        if (storage.Insert(key_, value_, life_time_))
        {
            std::cout << "> OK" << std::endl;
        }
        else
        {
            std::cout << "> Key '" << key_ << "' already exists." << std::endl;
        }
    }

private:
    key_type key_;
    mapped_type value_;
    int64_t life_time_{ 0 };
};

template<class Container>
class GetCommand : public Command<Container>
{
public:
    using key_type = typename Command<Container>::key_type;

    explicit GetCommand(std::istream& is)
    {
        is >> key_;
    }

    void Execute(Container& storage) override
    {
        try
        {
            auto value = storage.GetValue(key_);
            std::cout << "> " << value << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "> (null)" << std::endl;
        }
    }

private:
    key_type key_;
};

template<class Container>
class ExistsCommand : public Command<Container>
{
public:
    using key_type = typename Command<Container>::key_type;

    explicit ExistsCommand(std::istream& is)
    {
        is >> key_;
    }

    void Execute(Container& storage) override
    {
        std::cout << "> " << std::boolalpha << storage.Exists(key_) << std::endl;
    }

private:
    key_type key_;
};

template<class Container>
class DeleteCommand : public Command<Container>
{
public:
    using key_type = typename Command<Container>::key_type;

    explicit DeleteCommand(std::istream& is)
    {
        is >> key_;
    }

    void Execute(Container& storage) override
    {
        std::cout << "> " << std::boolalpha << storage.Erase(key_) << std::endl;
    }

private:
    key_type key_;
};

template<class Container>
class UpdateCommand : public Command<Container>
{
public:
    using key_type = typename Command<Container>::key_type;
    using mapped_type = typename Command<Container>::mapped_type;
    
    explicit UpdateCommand(std::istream& is)
    {
        is >> key_;
        is >> value_;
    }

    void Execute(Container& storage) override
    {
        if (storage.Update(key_, value_))
        {
            std::cout << "> OK" << std::endl;
        }
        else
        {
            std::cout << "The entry does not exist." << std::endl;
        }
    }

private:
    key_type key_;
    mapped_type value_;
};

template<class Container>
class KeysCommand : public Command<Container>
{
public:
    KeysCommand() = default;
    
    void Execute(Container& storage) override
    {
        auto keys = storage.Keys();

        if (!keys.empty())
        {
            for (std::size_t i = 0; i < keys.size(); ++i)
            {
                std::cout << i + 1 << ") " << keys[i] << std::endl;
            }
        }
        else
        {
            std::cout << "Not a single key was found." << std::endl;
        }
    }
};

template<class Container>
class RenameCommand : public Command<Container>
{
public:
    using key_type = typename Command<Container>::key_type;
    
    explicit RenameCommand(std::istream& is)
    {
        is >> key1_;
        is >> key2_;
    }

    void Execute(Container& storage) override
    {
        if (storage.Rename(key1_, key2_))
        {
            std::cout << "> OK" << std::endl;
        }
        else
        {
            std::cout << "The current key '" << key1_ << "' is not found or a new one '" << key2_ << "' already exists." << std::endl;
        }
    }

private:
    key_type key1_;
    key_type key2_;
};

template<class Container>
class TTLCommand : public Command<Container>
{
public:
    using key_type = typename Command<Container>::key_type;

    explicit TTLCommand(std::istream& is)
    {
        is >> key_;
    }

    void Execute(Container& storage) override
    {
        if (std::size_t ttl = storage.TTL(key_); ttl)
        {
            std::cout << "> " << ttl << std::endl;
        }
        else
        {
            std::cout << "> (null)" << std::endl;
        }
    }

private:
    key_type key_;
};

template<class Container>
class FindCommand : public Command<Container>
{
public:
    using mapped_type = typename Command<Container>::mapped_type;
    
    explicit FindCommand(std::istream& is)
    {
        is >> value_;
    }

    void Execute(Container& storage) override
    {
        auto keys = storage.Find(value_);

        if (!keys.empty())
        {
            for (std::size_t i = 0; i < keys.size(); ++i)
            {
                std::cout << "> " << i + 1 << ") " << keys[i] << std::endl;
            }
        }
        else
        {
            std::cout << "Not a single key was found." << std::endl;
        }
    }

private:
    mapped_type value_;
};

template<class Container>
class ShowAllCommand : public Command<Container>
{
public:
    ShowAllCommand() = default;
    
    void Execute(Container& storage) override
    {
        auto values = storage.ShowAll();

        if (!values.empty())
        {
            std::cout << std::setw(8) << "№ |";
            std::cout << std::setw(20) << "Last name |";
            std::cout << std::setw(20) << "First name |";
            std::cout << std::setw(20) << "Year |";
            std::cout << std::setw(20) << "City |";
            std::cout << std::setw(20) << "Coins |" << std::endl;
            
            for (std::size_t i = 0; i < values.size(); ++i)
            {
                std::cout << std::setw(5) << std::to_string(i + 1) + ")";
                std::cout << std::setw(20) << values[i].last_name;
                std::cout << std::setw(20) << values[i].first_name;
                std::cout << std::setw(20) << values[i].birth_year;
                std::cout << std::setw(20) << values[i].city;
                std::cout << std::setw(20) << values[i].coins << std::endl;
            }
        }
        else
        {
            std::cout << "Not a single entry was found." << std::endl;
        }
    }
};

template<class Container>
class UploadCommand : public Command<Container>
{
public:
    explicit UploadCommand(std::istream& is)
    {
        is >> path_;
    }

    void Execute(Container& storage) override
    {
        auto num_entries = storage.Upload(path_);

        if (num_entries)
        {
            std::cout << "> OK " << num_entries << std::endl;
        }
        else
        {
            std::cout << "Data could not be uploaded from this file." << std::endl;
        }
    }

private:
    std::filesystem::path path_;
};

template<class Container>
class ExportCommand : public Command<Container>
{
public:
    explicit ExportCommand(std::istream& is)
    {
        is >> path_;
    }

    void Execute(Container& storage) override
    {
        auto num_entries = storage.Export(path_);

        if (num_entries)
        {
            std::cout << "> OK " << num_entries << std::endl;
        }
        else
        {
            std::cout << "Data cannot be exported to this file." << std::endl;
        }
    }

private:
    std::filesystem::path path_;
};

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_COMMON_COMMAND_H_
