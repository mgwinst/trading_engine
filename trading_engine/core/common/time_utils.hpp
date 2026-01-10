#include <chrono>

struct NanosecondTimeStamp
{
    NanosecondTimeStamp()
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
        timestamp_ = nanoseconds_since_epoch.count();
    }

    long long timestamp_;
};