#pragma once

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <fmt/format.h>

namespace clt {
inline std::string join(const uint8_t *in, const size_t &n) {
    std::stringstream sst;
    if (n == 0) {
        return "";
    }
    sst << fmt::format("{:>02x}", (uint_fast64_t(in[0]) & 0xffull));
    if (n > 1) {
        for (size_t i = 1; i < n; i++) {
            sst << fmt::format(":{:>02x}", (uint_fast64_t(in[i]) & 0xffull));
        }
    }
    return sst.str();
}
template <class T, size_t N> inline auto join(const T (&in)[N]) {
    return join(in, N);
}
template <class T, size_t N> inline auto join(const std::array<T, N> &in) {
    return join(in.data(), N);
}
namespace aes128 {
constexpr size_t block_bytes = 16;
constexpr size_t key_bytes = 16;
constexpr uint8_t zero_key[key_bytes] = {0};
constexpr size_t num_rounds = 10;
} // namespace aes128
class AES128 {
    uint8_t expanded_keys_[aes128::block_bytes * 2 * aes128::num_rounds];

public:
    using block_t = std::array<uint8_t, aes128::block_bytes>;
    using key_t = std::array<uint8_t, aes128::key_bytes>;
    explicit AES128(const uint8_t *key) noexcept;
    explicit AES128() : AES128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const AES128 &x);
    void enc(uint8_t *out, const uint8_t *in) const noexcept;
    void enc(uint8_t *out, const uint8_t *in, const size_t num_blocks) const
        noexcept;
    void dec(uint8_t *out, const uint8_t *in) const noexcept;
    void dec(uint8_t *out, const uint8_t *in, const size_t num_blocks) const
        noexcept;
};
class MMO128 {
    uint8_t expanded_keys_[aes128::block_bytes * (aes128::num_rounds + 1)];

public:
    explicit MMO128(const uint8_t *key) noexcept;
    explicit MMO128() : MMO128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const MMO128 &x);
    void hash(uint8_t *out, const uint8_t *in) const noexcept;
    void hash(uint8_t *out, const uint8_t *in, const size_t num_blocks) const
        noexcept;
};
} // namespace clt

#include "aes-ni_impl/aes-ni_inline.hpp"

// vim: set expandtab :
