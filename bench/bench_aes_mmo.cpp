#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

template <class T, class U, class V>
inline void do_aes128_mmo(T &out, const U &in, const V &hash)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const double elapsed_time =
        measure([&]() { hash(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("aes128mmo,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T> inline void do_hash_iteration(const T &hash)
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff, hash_buff;
    buff.reserve(stop_byte_size);
    hash_buff.reserve(stop_byte_size);
    while (current < stop_byte_size) {
        buff.resize(current);
        hash_buff.resize(buff.size());
        for (size_t i = 0; i < num_loop; i++) {
            clt::init(buff, current);
            do_aes128_mmo(hash_buff, buff, hash);
        }
        current <<= 1;
    }
    buff.resize(stop_byte_size);
    hash_buff.resize(buff.size());
    for (size_t i = 0; i < num_loop; i++) {
        clt::init(buff, stop_byte_size);
        do_aes128_mmo(hash_buff, buff, hash);
    }
}

int main()
{
    AES128::key_t key;
    gen_key(key);
    clt::MMO128 hash(key.data());
    do_hash_iteration(hash);
    return 0;
}
