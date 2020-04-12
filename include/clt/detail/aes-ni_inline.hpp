#pragma once

#include <iostream>

#include "../util.hpp"

namespace clt {
inline std::ostream &operator<<(std::ostream &ost, const AES128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "AES128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << "[";
        ost << join(&x.expanded_keys_[i * aes128::block_bytes],
                    aes128::block_bytes);
        ost << "]";
    }
    ost << "]";
    return ost;
}

inline std::ostream &operator<<(std::ostream &ost, const MMO128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "MMO128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << "[";
        ost << join(&x.expanded_keys_[i * aes128::block_bytes],
                    aes128::block_bytes);
        ost << "]";
    }
    ost << "]";
    return ost;
}

inline std::ostream &operator<<(std::ostream &ost, const AESPRF128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "AESPRF128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << "[";
        ost << join(&x.expanded_keys_[i * aes128::block_bytes],
                    aes128::block_bytes);
        ost << "]";
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
        ost << "[";
        ost << join(&x.prf_.expanded_keys_[i], aes128::block_bytes);
        ost << "]";
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
