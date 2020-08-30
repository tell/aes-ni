#include <thread>
#ifdef _OPENMP
#include <omp.h>
#endif

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
        }
    }
}

int main()
{
    print_diagnosis();
    print_omp_diagnosis();
    do_shuffle_ys_iteration();
    return 0;
}
