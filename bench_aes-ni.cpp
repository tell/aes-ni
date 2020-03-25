#include <ctime>
#include <chrono>
#include <vector>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <clt/aes-ni.hpp>
#include <clt/rng.hpp>

#include "bench/bench_config.hpp"

using namespace std;

template <class Func> inline auto measure(Func &&f)
{
    return clt::bench::measure(forward<Func>(f));
}

constexpr size_t start_byte_size = 1 << 10;
constexpr size_t stop_byte_size = 1 << 30;

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
            clt::init(buff, current);
            do_prf(prf_buff, buff, prf);
        }
        current <<= 1;
    }
    buff.resize(stop_byte_size);
    prf_buff.resize(buff.size());
    for (size_t i = 0; i < num_loop; i++) {
        clt::init(buff, stop_byte_size);
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
    clt::AES128::key_t key;
    copy(begin(clt::aes128::zero_key), end(clt::aes128::zero_key), begin(key));
    clt::gen_key(key);
    fmt::print(cerr, "key = {}\n", clt::join(key));
    constexpr size_t buff_bytes = 1 << 10;
    vector<uint8_t> buff(buff_bytes);
    clt::init(buff, buff_bytes);
    vector<uint8_t> enc_buff(buff.size()), dec_buff(buff.size());
    assert((buff.size() % clt::aes128::block_bytes) == 0);
    assert((enc_buff.size() % clt::aes128::block_bytes) == 0);
    assert((dec_buff.size() % clt::aes128::block_bytes) == 0);
    clt::AES128 cipher(key.data());
    fmt::print(cerr, "cipher = {}\n", cipher);
    fmt::print("mode,bytes,bytes/s,blocks/s\n");
    clt::AESPRF128 prf(key.data());
    do_prf_iteration(prf);
    do_prfctr_iteration(prf);
    return 0;
}
