#pragma once

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
template <class T, size_t N> inline std::string join(const T (&in)[N]) {
    return join(in, N);
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
    explicit AES128(const uint8_t *key) noexcept;
    explicit AES128() : AES128(aes128::zero_key) {}
    friend std::ostream &operator<<(std::ostream &ost, const AES128 &x);
    void enc(uint8_t *out, const uint8_t *in) const noexcept;
    void enc(uint8_t *out, const uint8_t *in, const size_t iter_n) const
        noexcept;
    void dec(uint8_t *out, const uint8_t *in) const noexcept;
    void dec(uint8_t *out, const uint8_t *in, const size_t iter_n) const
        noexcept;
};
} // namespace clt

#include "aes-ni_impl.hpp"

// vim: set expandtab :
