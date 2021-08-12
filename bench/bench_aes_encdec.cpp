#include <clt/aes-ni.hpp>
#include <clt/rng.hpp>
#include <clt/benchmark.hpp>

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

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
        init(buff);
        assert((enc_buff.size() % aes128::block_bytes) == 0);
        {
            const size_t num_blocks = enc_buff.size() / aes128::block_bytes;
            print_throughput("aes128enc", enc_buff.size(), [&]() {
                cipher.enc(enc_buff.data(), buff.data(), num_blocks);
            });
        }
#ifndef NDEBUG
        dec_buff.resize(enc_buff.size());
        assert((dec_buff.size() % clt::aes128::block_bytes) == 0);
        {
            const size_t num_blocks =
                dec_buff.size() / clt::aes128::block_bytes;
            print_throughput("aes128dec", dec_buff.size(), [&]() {
                cipher.dec(dec_buff.data(), enc_buff.data(), num_blocks);
            });
        }
        assert(eq_check(buff, dec_buff));
#endif
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    const AES128::key_t key = gen_key();
    fmt::print(cerr, "key = {:>02x}\n", fmt::join(key, ":"));
    AES128 cipher(key);
    do_enc_dec_iteration(cipher);
    return 0;
}
