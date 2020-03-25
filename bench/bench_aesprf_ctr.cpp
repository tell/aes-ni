#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

template <class T> inline void do_aesprf_ctr_iteration(const T &prf)
{
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        const size_t num_blocks = buff.size() / clt::aes128::block_bytes;
        print_throughput("aes128prf_ctr", buff.size(),
                         [&]() { prf.ctr_stream(buff.data(), num_blocks, 0); });
        current <<= 1;
    }
}

int main()
{
    const AES128::key_t key = gen_key();
    fmt::print(cerr, "key = {}\n", join(key));
    AESPRF128 prf(key.data());
    do_aesprf_ctr_iteration(prf);
    return 0;
}
