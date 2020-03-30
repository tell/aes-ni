#pragma once

#include <array>
#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>

#include <fmt/format.h>

namespace clt {
namespace aes128 {
constexpr size_t block_bytes = 16;
constexpr size_t key_bytes = 16;
constexpr uint8_t zero_key[key_bytes] = {0};
constexpr size_t num_rounds = 10;
inline auto bytes_to_blocks(const size_t num_bytes)
{
    size_t num_blocks = num_bytes / block_bytes;
    if ((num_bytes % block_bytes) == 0) {
        return num_blocks;
    } else {
        return num_blocks + 1;
    }
}
} // namespace aes128

class AES128 {
    uint8_t expanded_keys_[aes128::block_bytes * 2 * aes128::num_rounds];

public:
    using block_t = std::array<uint8_t, aes128::block_bytes>;
    using key_t = std::array<uint8_t, aes128::key_bytes>;
    explicit AES128(const void *key) noexcept;
    explicit AES128(const key_t &key) : AES128(key.data()) {}
    explicit AES128() : AES128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const AES128 &x);
    void enc(void *out, const void *in) const noexcept;
    void enc(void *out, const void *in, const size_t num_blocks) const noexcept;
    void dec(void *out, const void *in) const noexcept;
    void dec(void *out, const void *in, const size_t num_blocks) const noexcept;
    auto ctr_stream(void *out, const uint64_t num_blocks,
                    const uint64_t start_count) const noexcept
        -> decltype(num_blocks + start_count);
};

AES128::key_t gen_key();

class MMO128 {
    /**
     * CRH based on AES128 (MMO).
     * References:
     * - Guo et al., "Efficient and Secure Multiparty Computation from Fixed-Key Block Ciphers"
     * https://eprint.iacr.org/2019/074
     */
    uint8_t expanded_keys_[aes128::block_bytes * (aes128::num_rounds + 1)];

public:
    explicit MMO128(const void *key) noexcept;
    explicit MMO128() : MMO128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const MMO128 &x);
    void operator()(void *out, const void *in) const noexcept;
    void operator()(void *out, const void *in, const size_t num_blocks) const
        noexcept;
};

class AESPRF128 {
    /**
     * PRF based on AES128 (AES-PRF).
     * References:
     * - Mennink, Neves, "Optimal PRFs from Blockcipher Designs"
     * https://doi.org/10.13154/tosc.v2017.i3.228-252
     */
    uint8_t expanded_keys_[aes128::block_bytes * (aes128::num_rounds + 1)];

public:
    explicit AESPRF128(const void *key) noexcept;
    explicit AESPRF128() : AESPRF128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const AESPRF128 &x);
    void operator()(void *out, const void *in) const noexcept;
    void operator()(void *out, const void *in, const size_t num_blocks) const
        noexcept;
    auto ctr_stream(void *out, const uint64_t num_blocks,
                    const uint64_t start_count) const noexcept
        -> decltype(num_blocks + start_count);
};
} // namespace clt

#include "clt/aes-ni_inline.hpp"

// vim: set expandtab :
