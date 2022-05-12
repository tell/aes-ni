#pragma once

#include <iostream>

#include <fmt/format.h>

#include "../aes-ni.hpp"
#include "../util.hpp"

namespace clt {
namespace aes128 {
inline auto bytes_to_blocks(const size_t num_bytes)
{
    size_t num_blocks = num_bytes / block_bytes;
    if ((num_bytes % block_bytes) == 0) {
        return num_blocks;
    } else {
        return num_blocks + 1;
    }
}
inline auto allocate_byte_size(const size_t num_bytes)
{
    return bytes_to_blocks(num_bytes) * block_bytes;
}
} // namespace aes128

inline std::ostream &operator<<(std::ostream &ost, const AES128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "AES128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << fmt::format(
            "[{:>02x}]",
            fmt::join(&x.expanded_keys_[i * aes128::block_bytes],
                      &x.expanded_keys_[(i + 1) * aes128::block_bytes], ":"));
    }
    ost << "]";
    return ost;
}
inline AES128_CTR::AES128_CTR(const void *key) noexcept
    : cipher_(key), counter_(0)
{
}
inline std::ostream &operator<<(std::ostream &ost, const AES128_CTR &x)
{
    static_assert((sizeof(x.cipher_.expanded_keys_) % aes128::block_bytes) ==
                  0);
    constexpr size_t num_exp_keys =
        sizeof(x.cipher_.expanded_keys_) / aes128::block_bytes;
    ost << "AES128_CTR[";
    ost << fmt::format("counter={:d},", x.counter_);
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << fmt::format(
            "[{:>02x}]",
            fmt::join(&x.cipher_.expanded_keys_[i * aes128::block_bytes],
                      &x.cipher_.expanded_keys_[(i + 1) * aes128::block_bytes],
                      ":"));
    }
    ost << "]";
    return ost;
}
inline void AES128_CTR::operator()(void *out, const size_t num_bytes) noexcept
{
    counter_ = cipher_.ctr_byte_stream(out, num_bytes, counter_);
}

inline std::ostream &operator<<(std::ostream &ost, const MMO128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "MMO128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << fmt::format(
            "[{:>02x}]",
            fmt::join(&x.expanded_keys_[i * aes128::block_bytes],
                      &x.expanded_keys_[(i + 1) * aes128::block_bytes], ":"));
    }
    ost << "]";
    return ost;
}
inline std::ostream &operator<<(std::ostream &ost, const MMO128_CTR &x)
{
    static_assert((sizeof(x.prf_.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.prf_.expanded_keys_) / aes128::block_bytes;
    ost << "MMO128_CTR[";
    ost << fmt::format("counter={:d},", x.counter_);
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << fmt::format(
            "[{:>02x}]",
            fmt::join(&x.prf_.expanded_keys_[i * aes128::block_bytes],
                      &x.prf_.expanded_keys_[(i + 1) * aes128::block_bytes],
                      ":"));
    }
    ost << "]";
    return ost;
}
inline void MMO128_CTR::operator()(void *out, const size_t num_bytes) noexcept
{
    counter_ = prf_.ctr_byte_stream(out, num_bytes, counter_);
}

inline std::ostream &operator<<(std::ostream &ost, const AESPRF128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "AESPRF128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << fmt::format(
            "[{:>02x}]",
            fmt::join(&x.expanded_keys_[i * aes128::block_bytes],
                      &x.expanded_keys_[(i + 1) * aes128::block_bytes], ":"));
    }
    ost << "]";
    return ost;
}
inline AESPRF128_CTR::AESPRF128_CTR(const void *key) noexcept
    : prf_(key), counter_(0)
{
}
inline std::ostream &operator<<(std::ostream &ost, const AESPRF128_CTR &x)
{
    static_assert((sizeof(x.prf_.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.prf_.expanded_keys_) / aes128::block_bytes;
    ost << "AESPRF128_CTR[";
    ost << fmt::format("counter={:d},", x.counter_);
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << fmt::format(
            "[{:>02x}]",
            fmt::join(&x.prf_.expanded_keys_[i * aes128::block_bytes],
                      &x.prf_.expanded_keys_[(i + 1) * aes128::block_bytes],
                      ":"));
    }
    ost << "]";
    return ost;
}
inline void AESPRF128_CTR::operator()(void *out,
                                      const size_t num_bytes) noexcept
{
    counter_ = prf_.ctr_byte_stream(out, num_bytes, counter_);
}
} // namespace clt
