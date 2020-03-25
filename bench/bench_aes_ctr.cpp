#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

template <class T> inline void do_aesprf_ctr_iteration(const T &cipher)
{
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        assert((buff.size() % clt::aes128::block_bytes) == 0);
        const size_t num_blocks = buff.size() / clt::aes128::block_bytes;
        print_throughput("aesprf128_ctr", buff.size(), [&]() {
            cipher.ctr_stream(buff.data(), num_blocks, 0);
        });
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    const AES128::key_t key = gen_key();
    fmt::print(cerr, "key = {}\n", join(key));
    AES128 cipher(key);
    do_aesprf_ctr_iteration(cipher);
    return 0;
}
