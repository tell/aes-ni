#include <spdlog/spdlog.h>

#include <clt/aes-ni.hpp>
#include <clt/statistics.hpp>
#include <clt/benchmark.hpp>

using namespace std;
using namespace clt;
using namespace clt::bench;

inline void do_aesprf_ctr_iteration()
{
    const AES128::key_t key = gen_key();
    fmt::print(cerr, "key = {}\n", join(key));
    if (!check_random_bytes(key)) {
        spdlog::warn("Skew key.");
    }
    AES128 cipher(key);
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        assert((buff.size() % clt::aes128::block_bytes) == 0);
        const size_t num_blocks = buff.size() / clt::aes128::block_bytes;
        print_throughput("aes128_ctr", buff.size(), [&]() {
            cipher.ctr_stream(buff.data(), num_blocks, 0);
        });
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    do_aesprf_ctr_iteration();
    return 0;
}
