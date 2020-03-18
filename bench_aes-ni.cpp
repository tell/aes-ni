#include <ctime>
#include <chrono>
#include <random>
#include <vector>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "aes-ni.hpp"

using namespace std;
using namespace std::chrono;

using sec_spec = microseconds;
constexpr double time_scale = 1e-6;

template <class Func> auto measure_walltime(Func &&f) {
    const auto start = system_clock::now();
    f();
    const auto stop = system_clock::now();
    return duration_cast<sec_spec>(stop - start).count() * time_scale;
}

template <class Func> auto measure_cputime(Func &&f) {
    const double start = clock();
    f();
    const double stop = clock();
    return (stop - start) / CLOCKS_PER_SEC;
}

void init(vector<uint8_t> &buff) {
    array<seed_seq::result_type, mt19937::state_size> seed_data;
    random_device seed_gen;
    generate(seed_data.begin(), seed_data.end(), ref(seed_gen));
    seed_seq seq(seed_data.begin(), seed_data.end());
    std::mt19937 engine(seq);
    generate(begin(buff), end(buff), ref(engine));
}

void gen_key(clt::AES128::key_t &key) {
    using result_t = random_device::result_type;
    static_assert((sizeof(decltype(key)) % sizeof(result_t)) == 0);
    constexpr size_t width = sizeof(decltype(key)) / sizeof(result_t);
    random_device engine;
    auto *p_out = reinterpret_cast<result_t *>(key.data());
    for (size_t i = 0; i < width; i++) {
        p_out[i] = engine();
    }
}

template <class T, class U, class V>
void do_enc(T &out, const U &in, const V &cipher) {
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_blocks = out.size() / clt::aes128::block_bytes;
    const auto elapsed_time = measure_cputime(
        [&]() { cipher.enc(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = out.size() / elapsed_time;
    fmt::print(" enc: {:.5e} bytes/s\n", bytes_per_sec);
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print(" enc: {:.5e} blocks/s\n", blocks_per_sec);
}

template <class T, class U, class V>
void do_dec(T &out, const U &in, const V &cipher) {
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_blocks = out.size() / clt::aes128::block_bytes;
    const double elapsed_time = measure_cputime(
        [&]() { cipher.dec(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = out.size() / elapsed_time;
    fmt::print(" dec: {:.5e} bytes/s\n", bytes_per_sec);
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print(" dec: {:.5e} blocks/s\n", blocks_per_sec);
}

template <class T, class U, class V>
void do_hash(T &out, const U &in, const V &hash) {
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_blocks = out.size() / clt::aes128::block_bytes;
    const double elapsed_time = measure_cputime(
        [&]() { hash.hash(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = out.size() / elapsed_time;
    fmt::print("hash: {:.5e} bytes/s\n", bytes_per_sec);
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("hash: {:.5e} blocks/s\n", blocks_per_sec);
}

int main() {
    fmt::print("CLOCKS_PER_SEC = {}\n", CLOCKS_PER_SEC);
    clt::AES128::key_t key;
    gen_key(key);
    fmt::print("key = {}\n", clt::join(key));
    constexpr size_t buff_bytes = 1 << 30;
    vector<uint8_t> buff(buff_bytes);
    init(buff);
    vector<uint8_t> enc_buff(buff.size()), dec_buff(buff.size());
    assert((buff.size() % clt::aes128::block_bytes) == 0);
    assert((enc_buff.size() % clt::aes128::block_bytes) == 0);
    assert((dec_buff.size() % clt::aes128::block_bytes) == 0);
    clt::AES128 cipher(key.data());
    fmt::print("cipher = {}\n", cipher);
    do_enc(enc_buff, buff, cipher);
    do_dec(dec_buff, enc_buff, cipher);
    fmt::print("     buff[:10] = {}\n", clt::join(buff.data(), 10));
    fmt::print(" dec_buff[:10] = {}\n", clt::join(dec_buff.data(), 10));
    for (size_t i = 0; i < buff_bytes; i++) {
        if (buff[i] != dec_buff[i]) {
            fmt::print("ERROR!! at i = {x}, buff[i] = {x}, dec_buff[i] = {x}\n",
                       i, buff[i], dec_buff[i]);
            abort();
        }
    }
    vector<uint8_t> hash_buff(buff.size());
    clt::MMO128 hash(key.data());
    fmt::print("hash = {}\n", hash);
    do_hash(hash_buff, buff, hash);
    fmt::print("hash_buff[:10] = {}\n", clt::join(hash_buff.data(), 10));
    for (size_t i = 0; i < buff_bytes; i++) {
        if ((buff[i] ^ enc_buff[i]) != hash_buff[i]) {
            fmt::print(
                "ERROR!! at i = {x}, buff[i] ^ enc_buff[i] = {x}, hash_buff[i] = {x}\n",
                i, buff[i] ^ enc_buff[i], hash_buff[i]);
            abort();
        }
    }
    return 0;
}