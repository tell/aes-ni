#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

template <class T, class U, class V>
void do_enc(T &out, const U &in, const V &cipher)
{
    assert((out.size() % aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / aes128::block_bytes;
    const auto elapsed_time =
        measure([&]() { cipher.enc(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("enc,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T> void do_enc_iteration(const T &cipher)
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff, enc_buff;
    buff.reserve(stop_byte_size);
    enc_buff.reserve(stop_byte_size);
    while (current < stop_byte_size) {
        buff.resize(current);
        enc_buff.resize(buff.size());
        for (size_t i = 0; i < num_loop; i++) {
            init(buff, current);
            do_enc(enc_buff, buff, cipher);
        }
        current <<= 1;
    }
    buff.resize(current);
    enc_buff.resize(buff.size());
    for (size_t i = 0; i < num_loop; i++) {
        init(buff, stop_byte_size);
        do_enc(enc_buff, buff, cipher);
    }
}

int main()
{
    AES128::key_t key;
    gen_key(key);
    AES128 cipher(key);
    do_enc_iteration(cipher);
    return 0;
}