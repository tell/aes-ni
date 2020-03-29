#include <clt/benchmark.hpp>

namespace clt {
namespace bench {

namespace {
[[maybe_unused]] inline auto measure_walltime(const std::function<void()> &func)
{
    using sec_spec = std::chrono::microseconds;
    constexpr double time_scale = 1e-6;

    const auto start = std::chrono::system_clock::now();
    (void)func();
    const auto stop = std::chrono::system_clock::now();
    return std::chrono::duration_cast<sec_spec>(stop - start).count() *
           time_scale;
}

[[maybe_unused]] inline auto measure_cputime(const std::function<void()> &func)
{
    const double start = clock();
    (void)func();
    const double stop = clock();
    return (stop - start) / CLOCKS_PER_SEC;
}
} // namespace

double measure_static(const std::function<void()> &func)
{
    return measure_walltime(func);
}

} // namespace bench
} // namespace clt
