#include <clt/aes-ni.hpp>
#include <clt/statistics.hpp>
#include <clt/benchmark.hpp>

using namespace std;
using namespace clt;
using namespace clt::bench;

inline void do_aesprf_ctr_iteration()
{
    const AES128::key_t key = gen_key();
    fmt::print(cerr, "key = {}\n", join(key, ":"));
    AESPRF128 prf(key.data());
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(aes128::allocate_byte_size(stop_byte_size));
    while (current <= stop_byte_size) {
        const size_t num_blocks = aes128::bytes_to_blocks(current);
        buff.resize(aes128::allocate_byte_size(current));
        print_throughput("aes128prf_ctr", buff.size(),
                         [&]() { prf.ctr_stream(buff.data(), num_blocks, 0); });
        if (!check_random_bytes(buff)) {
            fmt::print(cerr, "WARN: Unexpected statistics.\n");
        }
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    do_aesprf_ctr_iteration();
    return 0;
}
