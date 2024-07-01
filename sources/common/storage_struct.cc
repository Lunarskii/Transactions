#include "common/storage_struct.h"

namespace s21
{

std::ostream& operator<<(std::ostream& os, const Value& value)
{
    os << value.last_name << " ";
    os << value.first_name << " ";
    os << ((value.birth_year == -1) ? "-" : std::to_string(value.birth_year)) << " ";
    os << value.city << " ";
    os << ((value.coins == -1) ? "-" : std::to_string(value.coins));
    
    return os;
}

std::istream& operator>>(std::istream& is, Value& value)
{
    is >> value.last_name;
    is >> value.first_name;
    is >> value.birth_year;
    if (value.birth_year < 0) throw std::runtime_error("The year of birth cannot be less than 0.");
    is >> value.city;
    is >> value.coins;
    if (value.coins < 0) throw std::runtime_error("The number of coins cannot be less than 0.");

    if (is.fail())
    {
        throw std::runtime_error("Invalid data.");
    }
    
    return is;
}

} // namespace s21
