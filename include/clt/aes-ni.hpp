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
inline auto bytes_to_blocks(const size_t num_bytes);
inline auto allocate_byte_size(const size_t num_bytes);
} // namespace aes128

class AES128_CTR;
class AES128 {
    uint8_t expanded_keys_[aes128::block_bytes * 2 * aes128::num_rounds];

public:
    using block_t = std::array<uint8_t, aes128::block_bytes>;
    using key_t = std::array<uint8_t, aes128::key_bytes>;
    explicit AES128(const void *key) noexcept;
    explicit AES128(const key_t &key) noexcept : AES128(key.data()) {}
    explicit AES128() : AES128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const AES128 &x);
    friend std::ostream &operator<<(std::ostream &ost, const AES128_CTR &x);
    void enc(void *out, const void *in) const noexcept;
    void enc(void *out, const void *in, const size_t num_blocks) const noexcept;
    void dec(void *out, const void *in) const noexcept;
    void dec(void *out, const void *in, const size_t num_blocks) const noexcept;
    auto ctr_stream(void *out, const uint64_t num_blocks,
                    const uint64_t start_count) const noexcept
        -> decltype(num_blocks + start_count);
    auto ctr_byte_stream(void *out, const uint64_t num_bytes,
                         const uint64_t start_count) const noexcept
        -> decltype(num_bytes + start_count);
};

class AES128_CTR {
    AES128 cipher_;
    uint64_t counter_;

public:
    explicit AES128_CTR(const void *key) noexcept;
    explicit AES128_CTR() noexcept : AES128_CTR(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const AES128_CTR &x);
    void operator()(void *out, const size_t num_bytes) noexcept;
    void set_counter(const uint64_t counter) noexcept { counter_ = counter; };
    auto get_counter() const noexcept { return counter_; };
};

AES128::key_t gen_key();

class MMO128_CTR;

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
    explicit MMO128(const AES128::key_t &key) noexcept : MMO128(key.data()) {}
    explicit MMO128() noexcept : MMO128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const MMO128 &x);
    friend std::ostream &operator<<(std::ostream &ost, const MMO128_CTR &x);
    void operator()(void *out, const void *in) const noexcept;
    void operator()(void *out, const void *in,
                    const size_t num_blocks) const noexcept;
    auto ctr_stream(void *out, const uint64_t num_blocks,
                    const uint64_t start_count) const noexcept
        -> decltype(num_blocks + start_count);
    auto ctr_byte_stream(void *out, const uint64_t num_bytes,
                         const uint64_t start_count) const noexcept
        -> decltype(num_bytes + start_count);
};

class MMO128_CTR {
    MMO128 prf_;
    uint64_t counter_;

public:
    explicit MMO128_CTR(const void *key) noexcept : prf_(key), counter_(0) {}
    explicit MMO128_CTR(const AES128::key_t &key) noexcept
        : prf_(key), counter_{0}
    {
    }
    explicit MMO128_CTR() noexcept : MMO128_CTR(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const MMO128_CTR &x);
    void operator()(void *out, const size_t num_bytes) noexcept;
    void set_counter(const uint64_t counter) noexcept { counter_ = counter; };
    auto get_counter() const noexcept { return counter_; };
};

class AESPRF128_CTR;
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
    explicit AESPRF128(const AES128::key_t &key) noexcept
        : AESPRF128(key.data()){};
    explicit AESPRF128() noexcept : AESPRF128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const AESPRF128 &x);
    friend std::ostream &operator<<(std::ostream &ost, const AESPRF128_CTR &x);
    void operator()(void *out, const void *in) const noexcept;
    void operator()(void *out, const void *in,
                    const size_t num_blocks) const noexcept;
    auto ctr_stream(void *out, const uint64_t num_blocks,
                    const uint64_t start_count) const noexcept
        -> decltype(num_blocks + start_count);
    auto ctr_byte_stream(void *out, const uint64_t num_bytes,
                         const uint64_t start_count) const noexcept
        -> decltype(num_bytes + start_count);
};

class AESPRF128_CTR {
    AESPRF128 prf_;
    uint64_t counter_;

public:
    explicit AESPRF128_CTR(const void *key) noexcept;
    explicit AESPRF128_CTR(const AES128::key_t &key) noexcept
        : prf_(key), counter_(0){};
    explicit AESPRF128_CTR() noexcept : AESPRF128_CTR(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const AESPRF128_CTR &x);
    void operator()(void *out, const size_t num_bytes) noexcept;
    void set_counter(const uint64_t counter) noexcept { counter_ = counter; };
    auto get_counter() const noexcept { return counter_; };
};
} // namespace clt

#include "detail/aes-ni_inline.hpp"
#include "detail/aen-ni_encdec_impl.hpp"
#include "detail/aes-ni_key-exp_impl.hpp"

// vim: set expandtab :
