#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

template <class T> inline void do_aesprf_iteration(const T &prf)
{
    size_t current = start_byte_size;
    vector<uint8_t> buff, prf_buff;
    buff.reserve(stop_byte_size);
    prf_buff.reserve(stop_byte_size);
    fmt::print("mode,bytes,bytes/s\n");
    while (current <= stop_byte_size) {
        buff.resize(current);
        prf_buff.resize(buff.size());
        assert((prf_buff.size() % clt::aes128::block_bytes) == 0);
        const size_t num_blocks = prf_buff.size() / clt::aes128::block_bytes;
        init(buff, current);
        print_throughput(
            "aes128prf",
            [&]() { prf(prf_buff.data(), buff.data(), num_blocks); },
            prf_buff.size());
        current <<= 1;
    }
}

int main()
{
    fmt::print(cerr, "CLOCKS_PER_SEC = {}\n", CLOCKS_PER_SEC);
    AES128::key_t key;
    gen_key(key);
    fmt::print(cerr, "key = {}\n", clt::join(key));
    clt::MMO128 hash(key.data());
    do_aesprf_iteration(hash);
    return 0;
}
