#include <algorithm>
#include <cmath>

#include <fmt/format.h>

#include <clt/aes-ni.hpp>
#include <clt/shuffle.hpp>
#include <clt/util_omp.hpp>
#include <clt/benchmark.hpp>

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

inline void do_shuffle_ys_iteration()
{
    using elem_t = uint32_t;
    constexpr size_t step = 1;
    constexpr size_t num_cases = 23;
    constexpr size_t num_samples = 10;
    const std::string fmt_str = "shuffle_ys_aesprf128,{},{:e}\n";
    fmt::print("mode,num_32bit_elems,seconds\n");
    for (size_t i = 0; i < num_cases; i++) {
        array<double, num_samples> time_samples;
        const size_t num_elems = start_byte_size << (i * step);
#pragma omp parallel for
        for (size_t j = 0; j < num_samples; j++) {
            AES128::key_t key = gen_key();
            AESPRF128_CTR prf(key.data());
            vector<elem_t> buff(num_elems);
            iota(begin(buff), end(buff), 0);
            const auto elapsed_time = measure_static(
                [&]() { shuffle(buff.data(), size(buff), prf); });
            dummy_call(reinterpret_cast<void *>(buff.data()));
            time_samples[j] = elapsed_time;
        }
        const auto time_median = median(time_samples);
        fmt::print(fmt_str, num_elems, time_median);
    }
}

inline void do_shuffle_ys_iteration_10()
{
    using elem_t = uint32_t;
    constexpr size_t start_i = 1;
    constexpr size_t stop_i = 8;
    constexpr size_t num_cases = stop_i - start_i + 1;
    constexpr size_t num_samples = 40;
    fmt::print("# Median of {} samples.\n", num_samples);
    const std::string fmt_str = "shuffle_ys_aesprf128,{},{:e}\n";
    fmt::print("mode,num_32bit_elems,seconds\n");
    for (size_t i = 0; i < num_cases; i++) {
        array<double, num_samples> time_samples;
        const auto num_elems = static_cast<size_t>(std::pow(10, start_i + i));
        for (size_t j = 0; j < num_samples; j++) {
            AES128::key_t key = gen_key();
            AESPRF128_CTR prf(key.data());
            vector<elem_t> buff(num_elems);
            const auto elapsed_time = measure_static(
                [&]() { shuffle(buff.data(), size(buff), prf); });
            dummy_call(reinterpret_cast<void *>(buff.data()));
            time_samples[j] = elapsed_time;
        }
        const auto time_median = median(time_samples);
        fmt::print(fmt_str, num_elems, time_median);
    }
}

int main()
{
    print_diagnosis();
    print_omp_diagnosis();
    do_shuffle_ys_iteration();
    do_shuffle_ys_iteration_10();
    return 0;
}
