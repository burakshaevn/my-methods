#include <iostream>
#include <chrono>
#include <thread> 

using namespace std::literals; 

#define PROFILE_CONCAT_INTERNAL(X, Y) X ## Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x)

class LogDuration {
public:
    using Clock = std::chrono::steady_clock;

    LogDuration(std::string operation_name)
        : operation_name_(operation_name)
    {}

    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        std::cerr << operation_name_ << ": "s << duration_cast<milliseconds>(dur).count() << " ms"s << std::endl;
    }

private:
    std::string operation_name_;
    const Clock::time_point start_time_ = Clock::now();
};