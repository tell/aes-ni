#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::bench;

template <class T, class U, class V>
inline void do_enc(T &out, const U &in, const V &cipher)
{
    assert((out.size() % aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / aes128::block_bytes;
    const auto elapsed_time =
        measure([&]() { cipher.enc(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("aes128enc,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T, class U, class V>
inline void do_dec(T &out, const U &in, const V &cipher)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const double elapsed_time =
        measure([&]() { cipher.dec(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("aes128dec,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T>
inline bool eq_check(const vector<T> &buff0, const vector<T> &buff1)
{
    assert(buff0.size() == buff1.size());
    const size_t num_elems = buff0.size();
    for (size_t i = 0; i < num_elems; i++) {
        if (buff0[i] != buff1[i]) {
            return false;
        }
    }
    return true;
}

template <class T> inline void do_enc_dec_iteration(const T &cipher)
{
    size_t current = start_byte_size;
    vector<uint8_t> buff, enc_buff;
    buff.reserve(stop_byte_size);
    enc_buff.reserve(stop_byte_size);
#ifndef NDEBUG
    fmt::print(cerr, "Decryption check is enabled.\n");
    vector<uint8_t> dec_buff;
    dec_buff.reserve(stop_byte_size);
#endif
    while (current <= stop_byte_size) {
        buff.resize(current);
        enc_buff.resize(buff.size());
        init(buff, current);
        do_enc(enc_buff, buff, cipher);
#ifndef NDEBUG
        dec_buff.resize(buff.size());
        do_dec(dec_buff, enc_buff, cipher);
        assert(eq_check(buff, dec_buff));
#endif
        current <<= 1;
    }
}

template <class T, class U> void do_aesctr(T &out, const U &cipher)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const double elapsed_time =
        measure([&]() { cipher.ctr_stream(out.data(), num_blocks, 0); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("aes128ctr,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T> void do_aesctr_iteration(const T &cipher)
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        for (size_t i = 0; i < num_loop; i++) {
            do_aesctr(buff, cipher);
        }
        current <<= 1;
    }
}

int main()
{
    AES128::key_t key;
    gen_key(key);
    AES128 cipher(key);
    do_enc_dec_iteration(cipher);
    return 0;
}
