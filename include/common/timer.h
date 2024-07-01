#ifndef TRANSACTIONS_INCLUDE_COMMON_TIMER_H_
#define TRANSACTIONS_INCLUDE_COMMON_TIMER_H_

#include <chrono>
#include <stdexcept>

namespace s21
{

template<class Unit = std::chrono::seconds, class Clock = std::chrono::high_resolution_clock>
class Timer
{
public:
    using unit_type = Unit;
    using clock_type = Clock;

public:
    Timer() = default;

    explicit Timer(int64_t life_time)
    {
        SetLifeTime(life_time);
    }

    Timer(const Timer& other)
        : life_time_(other.life_time_)
        , start_time_(other.start_time_)
        , end_time_(other.end_time_)
        , started_(other.started_)
        , ended_(other.ended_)
    {}

    Timer(Timer&& timer) noexcept
    {
        std::swap(life_time_, timer.life_time_);
        std::swap(start_time_, timer.start_time_);
        std::swap(end_time_, timer.end_time_);
        std::swap(started_, timer.started_);
        std::swap(ended_, timer.ended_);
    }

    Timer& operator=(const Timer& other)
    {
        if (this != &other)
        {
            new (this) Timer(other);
        }

        return *this;
    }

    Timer& operator=(Timer&& other) noexcept
    {
        if (this != &other)
        {
            new (this) Timer(std::move(other));
        }

        return *this;
    }

    void Start() noexcept
    {
        if (ended_)
        {
            Clear();
        }

        start_time_ = GetNewTime_();
        started_ = true;
    }

    void Stop()
    {
        if (!started_)
        {
            throw std::runtime_error("Timer has not been started");
        }

        end_time_ = GetNewTime_();
        started_ = false;
        ended_ = true;
    }

    [[nodiscard]] bool IsExpired() const
    {
        if (!started_)
        {
            throw std::runtime_error("Timer has not been started");
        }

        return std::chrono::duration_cast<unit_type>(GetNewTime_() - start_time_).count() >= life_time_;
    }

    void Clear() noexcept
    {
        start_time_ = std::chrono::time_point<clock_type>{};
        end_time_ = std::chrono::time_point<clock_type>{};
        started_ = false;
        ended_ = false;
    }

    typename unit_type::rep GetTime() const
    {
        if (!started_)
        {
            throw std::runtime_error("Timer has not been started");
        }

        if (!ended_)
        {
            return std::chrono::duration_cast<unit_type>(GetNewTime_() - start_time_).count();
        }
        return std::chrono::duration_cast<unit_type>(end_time_ - start_time_).count();
    }

    void SetLifeTime(int64_t life_time)
    {
        if (life_time <= 0)
        {
            throw std::invalid_argument("Life time must be greater than 0.");
        }

        life_time_ = life_time;
    }

    [[nodiscard]] int64_t GetLifeTime() const noexcept
    {
        return life_time_;
    }

    [[nodiscard]] int64_t GetRemainingTime() const
    {
        return GetLifeTime() - GetTime();
    }

    template <class Function, class... Args>
    std::chrono::milliseconds::rep MarkTime(std::size_t num_times, Function&& f, Args&&... args)
    {
        auto func = std::bind(std::forward<Function>(f), std::forward<Args>(args)...);

        auto start_time = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < num_times; ++i)
        {
            func();
        }
        auto end_time = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }

private:
    std::chrono::time_point<clock_type> GetNewTime_() const noexcept
    {
        return clock_type::now();
    }

private:
    int64_t life_time_{ 0 };
    std::chrono::time_point<clock_type> start_time_;
    std::chrono::time_point<clock_type> end_time_;
    bool started_{ false };
    bool ended_{ false };
};

} // namespace s21

#endif // TRANSACTIONS_INCLUDE_COMMON_TIMER_H_
