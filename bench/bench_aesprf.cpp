#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

template <class T, class U, class V>
void do_aesprf(T &out, const U &in, const V &prf)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const double elapsed_time =
        measure([&]() { prf(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("aes128prf,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T> void do_aesprf_iteration(const T &prf)
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff, prf_buff;
    buff.reserve(stop_byte_size);
    prf_buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        prf_buff.resize(buff.size());
        for (size_t i = 0; i < num_loop; i++) {
            clt::init(buff, current);
            do_aesprf(prf_buff, buff, prf);
        }
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
