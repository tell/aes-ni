#include <ctime>
#include <chrono>
#include <vector>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "aes-ni.hpp"
#include "rng.hpp"

using namespace std;
using namespace std::chrono;

using sec_spec = microseconds;
constexpr double time_scale = 1e-6;

namespace internal {
template <class Func> auto measure_walltime(Func &&f)
{
    const auto start = system_clock::now();
    f();
    const auto stop = system_clock::now();
    return duration_cast<sec_spec>(stop - start).count() * time_scale;
}

template <class Func> auto measure_cputime(Func &&f)
{
    const double start = clock();
    f();
    const double stop = clock();
    return (stop - start) / CLOCKS_PER_SEC;
}
} // namespace internal

template <class Func> auto measure(Func &&f)
{
    return internal::measure_walltime(forward<Func>(f));
}

constexpr size_t start_byte_size = 1 << 10;
constexpr size_t stop_byte_size = 1 << 30;

clt::rng::RNG rng;

void init(vector<uint8_t> &buff, const size_t num_bytes)
{
    buff.resize(num_bytes);
    const auto status = rng(buff.data(), num_bytes);
    if (!status) {
        fmt::print(cerr, "ERROR!! gen_key is failed.");
        abort();
    }
}

void gen_key(clt::AES128::key_t &key)
{
    if (!rng(key.data(), clt::aes128::key_bytes)) {
        fmt::print(cerr, "ERROR!! gen_key is failed.");
    }
}

template <class T, class U> void do_rng(T &out, U &rng)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const auto elapsed_time = measure([&]() { rng(out.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("rng,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

void do_rng_iterate()
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current < stop_byte_size) {
        buff.resize(current);
        for (size_t i = 0; i < num_loop; i++) {
            do_rng(buff, rng);
        }
        current <<= 1;
    }
    buff.resize(stop_byte_size);
    for (size_t i = 0; i < num_loop; i++) {
        do_rng(buff, rng);
    }
}

template <class T, class U, class V>
void do_enc(T &out, const U &in, const V &cipher)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
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

template <class T, class U, class V>
void do_dec(T &out, const U &in, const V &cipher)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const double elapsed_time =
        measure([&]() { cipher.dec(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("dec,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T, class U, class V>
void do_hash(T &out, const U &in, const V &hash)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const double elapsed_time =
        measure([&]() { hash(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("hash,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T> void do_hash_iteration(const T &hash)
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
            init(buff, current);
            do_hash(hash_buff, buff, hash);
        }
        current <<= 1;
    }
    buff.resize(stop_byte_size);
    hash_buff.resize(buff.size());
    for (size_t i = 0; i < num_loop; i++) {
        init(buff, stop_byte_size);
        do_hash(hash_buff, buff, hash);
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
    fmt::print("aesctr,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T> void do_aesctr_iteration(const T &cipher)
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current < stop_byte_size) {
        buff.resize(current);
        for (size_t i = 0; i < num_loop; i++) {
            do_aesctr(buff, cipher);
        }
        current <<= 1;
    }
    buff.resize(stop_byte_size);
    for (size_t i = 0; i < num_loop; i++) {
        do_aesctr(buff, cipher);
    }
}

template <class T, class U, class V>
void do_prf(T &out, const U &in, const V &prf)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const double elapsed_time =
        measure([&]() { prf(out.data(), in.data(), num_blocks); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("prf,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T> void do_prf_iteration(const T &prf)
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff, prf_buff;
    buff.reserve(stop_byte_size);
    prf_buff.reserve(stop_byte_size);
    while (current < stop_byte_size) {
        buff.resize(current);
        prf_buff.resize(buff.size());
        for (size_t i = 0; i < num_loop; i++) {
            init(buff, current);
            do_prf(prf_buff, buff, prf);
        }
        current <<= 1;
    }
    buff.resize(stop_byte_size);
    prf_buff.resize(buff.size());
    for (size_t i = 0; i < num_loop; i++) {
        init(buff, stop_byte_size);
        do_prf(prf_buff, buff, prf);
    }
}

template <class T, class U> void do_prfctr(T &out, const U &prf)
{
    assert((out.size() % clt::aes128::block_bytes) == 0);
    const size_t num_bytes = out.size();
    const size_t num_blocks = num_bytes / clt::aes128::block_bytes;
    const double elapsed_time =
        measure([&]() { prf.ctr_stream(out.data(), num_blocks, 0); });
    const auto bytes_per_sec = num_bytes / elapsed_time;
    const auto blocks_per_sec = num_blocks / elapsed_time;
    fmt::print("prfctr,{},{:.5e},{:.5e}\n", num_bytes, bytes_per_sec,
               blocks_per_sec);
}

template <class T> void do_prfctr_iteration(const T &prf)
{
    constexpr size_t num_loop = 1;
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current < stop_byte_size) {
        buff.resize(current);
        for (size_t i = 0; i < num_loop; i++) {
            do_prfctr(buff, prf);
        }
        current <<= 1;
    }
    buff.resize(stop_byte_size);
    for (size_t i = 0; i < num_loop; i++) {
        do_prfctr(buff, prf);
    }
}

int main()
{
    fmt::print(cerr, "CLOCKS_PER_SEC = {}\n", CLOCKS_PER_SEC);
    do_rng_iterate();
    clt::AES128::key_t key;
    copy(begin(clt::aes128::zero_key), end(clt::aes128::zero_key), begin(key));
    gen_key(key);
    fmt::print(cerr, "key = {}\n", clt::join(key));
    constexpr size_t buff_bytes = 1 << 10;
    vector<uint8_t> buff(buff_bytes);
    init(buff, buff_bytes);
    vector<uint8_t> enc_buff(buff.size()), dec_buff(buff.size());
    assert((buff.size() % clt::aes128::block_bytes) == 0);
    assert((enc_buff.size() % clt::aes128::block_bytes) == 0);
    assert((dec_buff.size() % clt::aes128::block_bytes) == 0);
    clt::AES128 cipher(key.data());
    fmt::print(cerr, "cipher = {}\n", cipher);
    fmt::print("mode,bytes,bytes/s,blocks/s\n");
    do_enc(enc_buff, buff, cipher);
    do_dec(dec_buff, enc_buff, cipher);
    fmt::print(cerr, "     buff[:10] = {}\n", clt::join(buff.data(), 10));
    fmt::print(cerr, " dec_buff[:10] = {}\n", clt::join(dec_buff.data(), 10));
    for (size_t i = 0; i < buff_bytes; i++) {
        if (buff[i] != dec_buff[i]) {
            fmt::print(cerr,
                       "ERROR!! at i = {x}, buff[i] = {x}, dec_buff[i] = {x}\n",
                       i, buff[i], dec_buff[i]);
            abort();
        }
    }
    do_enc_iteration(cipher);
    vector<uint8_t> hash_buff(buff.size());
    clt::MMO128 hash(key.data());
    fmt::print(cerr, "hash = {}\n", hash);
    do_hash(hash_buff, buff, hash);
    fmt::print(cerr, "hash_buff[:10] = {}\n", clt::join(hash_buff.data(), 10));
    for (size_t i = 0; i < buff_bytes; i++) {
        if ((buff[i] ^ enc_buff[i]) != hash_buff[i]) {
            fmt::print(
                cerr,
                "ERROR!! at i = {x}, buff[i] ^ enc_buff[i] = {x}, hash_buff[i] = {x}\n",
                i, buff[i] ^ enc_buff[i], hash_buff[i]);
            abort();
        }
    }
    do_hash_iteration(hash);
    do_aesctr_iteration(cipher);
    clt::AESPRF128 prf;
    do_prf_iteration(prf);
    do_prfctr_iteration(prf);
    return 0;
}
