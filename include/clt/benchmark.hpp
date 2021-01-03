#pragma once

#include <iostream>
#include <chrono>
#include <functional>

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace clt {
namespace bench {

constexpr size_t start_byte_size = 1 << 10;
constexpr size_t stop_byte_size = 1 << 30;

void dummy_call(void *);

template <class T> inline bool eq_check(T &&lvec, T &&rvec)
{
    if (std::size(lvec) != std::size(rvec)) {
        return false;
    }
    const auto vec_size = std::size(lvec);
    for (size_t i = 0; i < vec_size; i++) {
        if (lvec[i] != rvec[i]) {
            return false;
        }
    }
    return true;
}

inline void print_diagnosis()
{
    fmt::print(std::cerr, "CLOCKS_PER_SEC = {}\n", CLOCKS_PER_SEC);
}

double measure_static(const std::function<void()> &func);

inline bool print_throughput_call_once(const std::string &unit_label = "bytes")
{
    [[maybe_unused]] static bool call_once__ = ([&]() {
        fmt::print("mode,{0},sec,{0}/sec\n", unit_label);
        return true;
    })();
    return true;
}

template <class Func>
inline void print_throughput(const std::string &label, const size_t num_bytes,
                             Func &&func,
                             const std::string &unit_label = "bytes")
{
    [[maybe_unused]] static bool call_once__ =
        ([&]() { return print_throughput_call_once(unit_label); })();
    while (true) {
        const std::string format = label + ",{},{:e},{:e}\n";
        const auto elapsed_time = measure_static(std::function<void()>(func));
        const auto bytes_per_sec = num_bytes / elapsed_time;
        if (std::isinf(bytes_per_sec)) {
            fmt::print(std::cerr,
                       "Obtained throughput is the infinity, try again...\n");
        } else {
            fmt::print(format, num_bytes, elapsed_time, bytes_per_sec);
            break;
        }
    }
}

template <class Func>
inline void print_benchmark(Func &&func, const std::string &fmt_str,
                            const size_t num)
{
    while (true) {
        const auto elapsed_time = measure_static(std::function<void()>(func));
        const auto num_per_sec = num / elapsed_time;
        if (std::isinf(num_per_sec)) {
#pragma omp critical
            fmt::print(std::cerr,
                       "Obtained throughput is the infinity, try again...\n");
        } else {
#pragma omp critical
            fmt::print(fmt_str, num, elapsed_time, num_per_sec);
            break;
        }
    }
}

template <class T> double median(T &vs)
{
    using std::begin;
    using std::end;
    using std::size;
    using std::sort;

    sort(begin(vs), end(vs));
    const auto n = size(vs);
    if (n == 1) {
        return vs[0];
    }
    if (n == 2) {
        return (vs[0] + vs[1]) / 2;
    }
    const auto n2 = n / 2;
    if ((n % 2) == 0) {
        return (vs[n2] + vs[n2 + 1]) / 2;
    } else {
        return vs[n2 + 1];
    }
}

} // namespace bench
} // namespace clt
