#include <clt/aes-ni.hpp>
#include <clt/benchmark.hpp>

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

template <class T> inline void do_aesmmo_iteration(const T &hash)
{
    size_t current = start_byte_size;
    vector<uint8_t> buff, hash_buff;
    buff.reserve(stop_byte_size);
    hash_buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        hash_buff.resize(buff.size());
        init(buff);
        assert((hash_buff.size() % clt::aes128::block_bytes) == 0);
        const size_t num_blocks = hash_buff.size() / clt::aes128::block_bytes;
        print_throughput("aes128mmo", hash_buff.size(), [&]() {
            hash(hash_buff.data(), buff.data(), num_blocks);
        });
        current <<= 1;
    }
#if 0
    for (size_t i = 0; i < buff_bytes; i++) {
        if ((buff[i] ^ enc_buff[i]) != hash_buff[i]) {
            fmt::print(
                cerr,
                "ERROR!! at i = {x}, buff[i] ^ enc_buff[i] = {x}, hash_buff[i] = {x}\n",
                i, buff[i] ^ enc_buff[i], hash_buff[i]);
            abort();
        }
    }
#endif
}

int main()
{
    print_diagnosis();
    const AES128::key_t key = gen_key();
    fmt::print(cerr, "key = {}\n", join(key));
    MMO128 hash(key.data());
    do_aesmmo_iteration(hash);
    return 0;
}
