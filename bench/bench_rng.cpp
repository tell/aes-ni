#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

template <class T, class U> inline void do_rng(T &out, U &rng)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const auto elapsed_time = measure([&]() { rng(out.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("/dev/urandom,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

inline void do_rng_iteration()
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        for (size_t i = 0; i < num_loop; i++) {
            do_rng(buff, clt::rng::rng_global);
        }
        current <<= 1;
    }
}

int main()
{
    do_rng_iteration();
    return 0;
}
