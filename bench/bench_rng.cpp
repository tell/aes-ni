#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

inline void do_rng_iteration()
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    fmt::print("mode,bytes,bytes/s\n");
    while (current <= stop_byte_size) {
        buff.resize(current);
        assert((buff.size() % clt::aes128::block_bytes) == 0);
        const size_t num_blocks = buff.size() / clt::aes128::block_bytes;
        for (size_t i = 0; i < num_loop; i++) {
            print_throughput(
                "/dev/urandom",
                [&]() { clt::rng::rng_global(buff.data(), num_blocks); },
                buff.size());
        }
        current <<= 1;
    }
}

int main()
{
    fmt::print(cerr, "CLOCKS_PER_SEC = {}\n", CLOCKS_PER_SEC);
    do_rng_iteration();
    return 0;
}
