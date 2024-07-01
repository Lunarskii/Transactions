#ifndef TRANSACTIONS_INCLUDE_COMMON_DATA_GENERATOR_H_
#define TRANSACTIONS_INCLUDE_COMMON_DATA_GENERATOR_H_

#include <random>
#include <memory>
#include <algorithm>

namespace s21
{

class DataGenerator
{
public:
    using size_type = std::size_t;

public:
    DataGenerator();

    std::vector<std::string> GenerateNStrings(size_type num_strings, size_type length);
    std::string GenerateString(std::size_t length);
    std::vector<int> GenerateNNumbers(size_type num_numbers, int min, int max);
    int GenerateNumber(int min, int max);

private:
    const std::string characters_{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
    std::unique_ptr<std::mt19937> generator_;
};

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_COMMON_DATA_GENERATOR_H_
