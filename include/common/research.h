#ifndef TRANSACTIONS_INCLUDE_COMMON_RESEARCH_H_
#define TRANSACTIONS_INCLUDE_COMMON_RESEARCH_H_

#include <climits>

#include "storage_struct.h"
#include "timer.h"
#include "data_generator.h"

namespace s21
{

template <class Container>
class Research
{
public:
    using key_type = typename Container::key_type;
    using mapped_type = typename Container::mapped_type;
    using size_type = typename Container::size_type;

public:
    explicit Research(Container* container)
        : container_(container)
    {}

    double Run(size_type starting_num_elements, size_type num_times)
    {
        if (starting_num_elements < 100) starting_num_elements = 100;

        std::vector<std::pair<key_type, mapped_type>> entries = FillContainer_(starting_num_elements);
        return timer_.MarkTime(1, [&]()
        {
            TestGetValue_(num_times, entries);
            TestInsert_(num_times);
            TestErase_(num_times, entries);
            TestShowAll_(num_times);
            TestFind_(num_times, entries);
        }) / 5.0;
    }

private:
    std::vector<std::pair<key_type, mapped_type>> FillContainer_(size_type num_elements)
    {
        std::vector<std::pair<key_type, mapped_type>> entries;

        for (size_type i = 0; i < num_elements; ++i)
        {
            key_type key = generator_.GenerateString(default_string_length);
            mapped_type value{
                    generator_.GenerateString(default_string_length),
                    generator_.GenerateString(default_string_length),
                    generator_.GenerateNumber(default_integer_min, default_integer_max),
                    generator_.GenerateString(default_string_length),
                    generator_.GenerateNumber(default_integer_min, default_integer_max)
            };
            size_type life_time = generator_.GenerateNumber(default_integer_min, default_integer_max);

            container_->Insert(key, value, life_time);
            entries.push_back({ key, value });
        }

        return entries;
    }

    size_type TestGetValue_(size_type num_times, std::vector<std::pair<key_type, mapped_type>>& entries)
    {
        ShuffleEntries(entries);
        return timer_.MarkTime(num_times, [&]()
        {
            auto index = generator_.GenerateNumber(default_integer_min, entries.size() - 1);
            container_->GetValue(entries.at(index).first);
        });
    }

    double TestInsert_(size_type num_times)
    {
        return timer_.MarkTime(1, [&]()
        {
            FillContainer_(num_times);
        });
    }

    double TestErase_(size_type num_times, std::vector<std::pair<key_type, mapped_type>>& entries)
    {
        ShuffleEntries(entries);
        return timer_.MarkTime(num_times, [&]()
        {
            container_->Erase(entries.at(generator_.GenerateNumber(default_integer_min, entries.size() - 1)).first);
        });
    }

    double TestShowAll_(size_type num_times)
    {
        return timer_.MarkTime(num_times, [&]()
        {
            container_->Keys();
            container_->ShowAll();
        });
    }

    double TestFind_(size_type num_times, std::vector<std::pair<key_type, mapped_type>>& entries)
    {
        ShuffleEntries(entries);
        return timer_.MarkTime(num_times, [&]()
        {
            container_->Find(entries.at(generator_.GenerateNumber(default_integer_min, entries.size() - 1)).second);
        });
    }

    void ShuffleEntries(std::vector<std::pair<key_type, mapped_type>>& entries)
    {
        std::random_device rd_;
        std::mt19937 generator(rd_());
        std::shuffle(entries.begin(), entries.end(), generator);
    }

private:
    Container* container_;
    DataGenerator generator_;
    Timer<> timer_;
    static constexpr size_type default_string_length{1000};
    static constexpr int default_integer_min{0};
    static constexpr int default_integer_max{INT_MAX};
};

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_COMMON_RESEARCH_H_
