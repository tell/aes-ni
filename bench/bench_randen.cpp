#include <clt/aes-ni.hpp>
#include <clt/statistics.hpp>

#include <randen.h>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

inline void do_randen_iteration()
{
    using randen_result_t = uint64_t;
    static_assert((start_byte_size % sizeof(randen_result_t)) == 0);
    static_assert((stop_byte_size % sizeof(randen_result_t)) == 0);
    constexpr size_t start_uint64_size =
        start_byte_size / sizeof(randen_result_t);
    constexpr size_t stop_uint64_size =
        stop_byte_size / sizeof(randen_result_t);
    // NOTE: The constructor without argument initializes as all-zero-state.
    randen::Randen<randen_result_t> eng_randen;
    size_t current = start_uint64_size;
    vector<randen_result_t> buff;
    buff.reserve(stop_uint64_size);
    while (current <= stop_uint64_size) {
        buff.resize(current);
        print_throughput("randen", buff.size() * sizeof(randen_result_t),
                         [&]() {
                             for (size_t i = 0; i < current; i++) {
                                 buff[i] = eng_randen();
                             }
                         });
        if (check_random_bits(buff)) {
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
