#include "common/data_generator.h"

namespace s21
{

DataGenerator::DataGenerator()
{
    std::random_device rd_;
    generator_ = std::make_unique<std::mt19937>(rd_());
}

std::vector<std::string> DataGenerator::GenerateNStrings(size_type num_strings, size_type length)
{
    std::vector<std::string> strings;

    for (size_type i = 0; i < num_strings; ++i)
    {
        strings.push_back(GenerateString(length));
    }

    return strings;
}

std::string DataGenerator::GenerateString(std::size_t length)
{
    static std::uniform_int_distribution<> distribution(0, static_cast<int>(characters_.size()) - 1);
    std::string random_string(length, '\0');
    std::generate_n(random_string.begin(), length, [&]()
    {
        return characters_[distribution(*generator_)];
    });

    return random_string;
}

std::vector<int> DataGenerator::GenerateNNumbers(size_type num_numbers, int min, int max)
{
    std::vector<int> numbers;

    for (size_type i = 0; i < num_numbers; ++i)
    {
        numbers.push_back(GenerateNumber(min, max));
    }

    return numbers;
}

int DataGenerator::GenerateNumber(int min, int max)
{
    std::uniform_int_distribution<> distribution(min, max);
    return distribution(*generator_);
}

}
