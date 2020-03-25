#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

inline void do_rng_iteration()
{
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        assert((buff.size() % clt::aes128::block_bytes) == 0);
        const size_t num_blocks = buff.size() / clt::aes128::block_bytes;
        print_throughput("/dev/urandom", buff.size(), [&]() {
            clt::rng::rng_global(buff.data(), num_blocks);
        });
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    do_rng_iteration();
    return 0;
}
