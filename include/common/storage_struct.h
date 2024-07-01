#ifndef TRANSACTIONS_INCLUDE_COMMON_STORAGE_STRUCT_H_
#define TRANSACTIONS_INCLUDE_COMMON_STORAGE_STRUCT_H_

#include <string>
#include <chrono>
#include <iostream>

namespace s21
{

struct Value final
{
    std::string last_name{ "-" };
    std::string first_name{ "-" };
    int birth_year{ -1 };
    std::string city{ "-" };
    int coins{ -1 };

    Value() = default;
    Value(const Value& other) = default;
    Value(Value&& other) = default;
    
    [[nodiscard]] bool IsDefault() const
    {
        return
            last_name == "-" &&
            first_name == "-" &&
            birth_year == -1 &&
            city == "-" &&
            coins == -1
        ;
    }
    
    Value& operator=(const Value& other)
    {
        if (this != &other)
        {
            last_name = (other.last_name != "-") ? other.last_name : last_name;
            first_name = (other.first_name != "-") ? other.first_name : first_name;
            birth_year = (other.birth_year != -1) ? other.birth_year : birth_year;
            city = (other.city != "-") ? other.city : city;
            coins = (other.coins != -1) ? other.coins : coins;
        }
        
        return *this;
    }
    
    [[nodiscard]] bool operator==(const Value& other) const
    {
        return
            (last_name == other.last_name || other.last_name == "-") &&
            (first_name == other.first_name || other.first_name == "-") &&
            (birth_year == other.birth_year || other.birth_year == -1) &&
            (city == other.city || other.city == "-") &&
            (coins == other.coins || other.coins == -1)
        ;
    }
    
    [[nodiscard]] bool operator!=(const Value& other) const
    {
        return !(*this == other);
    }
};

std::ostream& operator<<(std::ostream& os, const Value& value);
std::istream& operator>>(std::istream& is, Value& value);

}

#endif // TRANSACTIONS_INCLUDE_COMMON_STORAGE_STRUCT_H_
