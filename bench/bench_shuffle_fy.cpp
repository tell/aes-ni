#include <algorithm>

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
    fmt::print("mode,num_32bit_elems,seconds,num_32bit_elems/seconds\n");
    const std::string fmt_str = "shuffle_ys_aesprf128,{},{:e},{:e}\n";
    constexpr size_t step = 1;
    constexpr size_t num_loop = 23;
    constexpr size_t num_iteration = 10;
#pragma omp parallel for
    for (size_t j = 0; j < num_iteration; j++) {
#pragma omp parallel for
        for (size_t i = 0; i < num_loop; i++) {
            AES128::key_t key = gen_key();
            AESPRF128_CTR prf(key.data());
            const size_t current = start_byte_size << (i * step);
            vector<elem_t> buff(current);
            iota(begin(buff), end(buff), 0);
            print_benchmark([&]() { shuffle(buff.data(), size(buff), prf); },
                            fmt_str, size(buff));
            dummy_call(reinterpret_cast<void *>(buff.data()));
        }
    }
}

uint64_t power10(uint64_t x)
{
    uint64_t pi = 1;
    for (uint64_t i = 0; i < x; i++) {
        pi *= 10;
    }
    return pi;
}

template <size_t N> double median(array<double, N> &vs)
{
    sort(begin(vs), end(vs));
    constexpr auto Ndiv2 = N / 2;
    if ((N % 2) == 0) {
        return (vs[Ndiv2] + vs[Ndiv2 + 1]) / 2;
    } else {
        return vs[Ndiv2 + 1];
    }
}

inline void do_shuffle_ys_iteration_10()
{
    using elem_t = uint32_t;
    fmt::print("mode,num_32bit_elems,seconds\n");
    const std::string fmt_str = "shuffle_ys_aesprf128,{},{:e}\n";
    constexpr size_t start_i = 1;
    constexpr size_t stop_i = 8;
    constexpr size_t num_cases = stop_i - start_i + 1;
    constexpr size_t num_samples = 20;
    fmt::print("# Median of {} samples.\n", num_samples);
    for (size_t i = 0; i < num_cases; i++) {
        array<double, num_samples> time_samples;
        const auto num_elems = power10(start_i + i);
        for (size_t j = 0; j < num_samples; j++) {
            AES128::key_t key = gen_key();
            AESPRF128_CTR prf(key.data());
            vector<elem_t> buff(num_elems);
            const auto elapsed_time = measure_static(
                [&]() { shuffle(buff.data(), size(buff), prf); });
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
    // do_shuffle_ys_iteration();
    do_shuffle_ys_iteration_10();
    return 0;
}
