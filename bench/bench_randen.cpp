#include <random>

#include <clt/aes-ni.hpp>
#include <clt/statistics.hpp>
#include <clt/benchmark.hpp>

#include <randen.h>

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

inline void do_randen_iteration()
{
    std::random_device rdev;
    using randen_result_t = uint64_t;
    std::uniform_int_distribution<randen_result_t> dist;
    static_assert((start_byte_size % sizeof(randen_result_t)) == 0);
    static_assert((stop_byte_size % sizeof(randen_result_t)) == 0);
    constexpr size_t start_uint64_size =
        start_byte_size / sizeof(randen_result_t);
    constexpr size_t stop_uint64_size =
        stop_byte_size / sizeof(randen_result_t);
    // NOTE: The constructor without argument initializes as all-zero-state.
    const randen_result_t seed = dist(rdev);
    fmt::print(cerr, "seed = {:016x}\n", seed);
    if (check_random_bytes(seed)) {
        fmt::print(cerr, "WARNING: Skew seed, not fatal.\n");
    }
    randen::Randen<randen_result_t> eng_randen(seed);
    size_t current = start_uint64_size;
    vector<randen_result_t> buff;
    buff.reserve(stop_uint64_size);
    while (current <= stop_uint64_size) {
        buff.resize(current);
        print_throughput("randen", buff.size() * sizeof(randen_result_t),
                         [&]() {
                             const size_t buff_size = buff.size();
                             for (size_t i = 0; i < buff_size; i++) {
                                 buff[i] = eng_randen();
                             }
                         });
        if (check_random_bytes(buff)) {
            fmt::print(cerr, "WARNING: Outside statistics.\n");
        }
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    do_randen_iteration();
    return 0;
}
