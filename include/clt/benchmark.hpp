#pragma once

#include <iostream>
#include <chrono>

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace clt {
namespace bench {

constexpr size_t start_byte_size = 1 << 10;
constexpr size_t stop_byte_size = 1 << 30;

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

inline bool print_throughput_call_once()
{
    [[maybe_unused]] static bool call_once__ = ([]() {
        fmt::print("mode,bytes,sec,bytes/sec\n");
        return true;
    })();
    return true;
}

template <class Func>
inline void print_throughput(const std::string &label, const size_t num_bytes,
                             Func &&func)
{
    [[maybe_unused]] static bool call_once__ =
        ([]() { return print_throughput_call_once(); })();
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
} // namespace bench
} // namespace clt
