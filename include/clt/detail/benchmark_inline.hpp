#pragma once

#include <chrono>
#include <functional>

namespace clt {
namespace bench {
inline double measure_walltime_sec(const std::function<void()> &func)
{
    return measure_walltime<std::chrono::seconds>(func);
}
inline double measure_walltime_milli(const std::function<void()> &func)
{
    return measure_walltime<std::chrono::milliseconds>(func);
}
inline double measure_walltime_micro(const std::function<void()> &func)
{
    return measure_walltime<std::chrono::microseconds>(func);
}
inline double measure_walltime_nano(const std::function<void()> &func)
{
    return measure_walltime<std::chrono::nanoseconds>(func);
}
} // namespace bench
} // namespace clt
