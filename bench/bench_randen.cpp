#include <clt/aes-ni.hpp>
#include <randen.h>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

inline void do_randen_iteration()
{
    static_assert((start_byte_size % sizeof(uint64_t)) == 0);
    static_assert((stop_byte_size % sizeof(uint64_t)) == 0);
    constexpr size_t start_uint64_size = start_byte_size / sizeof(uint64_t);
    constexpr size_t stop_uint64_size = stop_byte_size / sizeof(uint64_t);
    randen::Randen<uint64_t> eng_randen;
    size_t current = start_uint64_size;
    vector<uint64_t> buff;
    buff.reserve(stop_uint64_size);
    while (current <= stop_uint64_size) {
        buff.resize(current);
        print_throughput("randen", buff.size() * sizeof(uint64_t), [&]() {
            for (size_t i = 0; i < current; i++) {
                buff[i] = eng_randen();
            }
        });
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    do_randen_iteration();
    return 0;
}
